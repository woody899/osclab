#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include "config.h"
#include "lib/dplist.h"
#include "datamgr.h"
#include "sbuffer.h"
#include "sensor_db.h"

typedef struct {
    u_int16_t roomId;
    sensor_id_t sensorId;
    double runningAvg;
    double lastFiveSum;
    int lastFiveCount;
    time_t lastModified;
} listElement;



//typedef struct {
//    sensor_id_t sensorNo;
//    sensor_value_t temperature;
//    sensor_ts_t timestamp;
//} sensorInfo;


void *element_copy(void *element);
void element_free(void **element);
int element_compare(void *x, void *y);


dplist_t *roomAndSensor = NULL;
dplist_t *binaryDataList = NULL;


void *element_copy(void *element) {
    listElement *copy = malloc(sizeof(listElement));
    assert(copy != NULL);
    *copy = *(listElement *)element;
    return (void *)copy;
}

void element_free(void **element) {
    free(*element);
    *element = NULL;
}

int element_compare(void *x, void *y) {
    listElement *elem1 = (listElement *)x;
    listElement *elem2 = (listElement *)y;
    if (elem1->roomId < elem2->roomId) return -1;
    if (elem1->roomId > elem2->roomId) return 1;
    return 0;
}


void datamgr_parse_sensor_files(FILE *fp_sensor_map, sbuffer_t* buffer, pthread_mutex_t *mutex) {
    if (!fp_sensor_map || !buffer) {
        fprintf(stderr, "File pointers are NULL\n");
        return;
    }


    // Initialize list pointers
    roomAndSensor = dpl_create(element_copy, element_free, element_compare);
    binaryDataList = dpl_create(element_copy, element_free, element_compare);

    // Parsing MAP files
    uint16_t num1, num2;
    int count = 0;
    listElement *element = malloc(sizeof(listElement));
    while (fscanf(fp_sensor_map, "%hu %hu", &num1, &num2) == 2) {
        element->roomId = num1;
        element->sensorId = num2;
        roomAndSensor = dpl_insert_at_index(roomAndSensor, element, count, true);
        count++;
    }

    // Parsing binary data, this time from the shared buffer.
    int i = 0;
    sensor_data_t *sensorLog;
    sensorLog = (sensor_data_t*)malloc(sizeof(sensor_data_t));
    while (sbuffer_remove(buffer, sensorLog) == SBUFFER_SUCCESS) {
        pthread_mutex_lock(mutex);
        if (sensorLog->mgrData == 0) {
            bool sensorExists = false;
            for (int k = 0; k < dpl_size(roomAndSensor); k++) {
                listElement *roomElement = dpl_get_element_at_reference(roomAndSensor,
                                                                        dpl_get_reference_at_index(roomAndSensor, k));
                if (roomElement->sensorId == sensorLog->id) {
                    sensorExists = true;
                    // Update running average and log if necessary
                    update_running_avg(sensorLog->id, sensorLog->value);
                    dpl_insert_at_index(binaryDataList, sensorLog, i, true);
                    i++;
                    break;
                }
            }
            if (!sensorExists) {
                printf("Sensor data with ID %hu not found in sensor map\n", sensorLog->id);
            }

            buffer->head->data.mgrData = 1;
        }
        pthread_mutex_unlock(mutex);
    }
    // Freeing resources
    free(sensorLog);
    free(element);
}

void datamgr_free() {
    if (roomAndSensor) {
        dpl_free(&roomAndSensor, true);
    }
    if (binaryDataList) {
        dpl_free(&binaryDataList, true);
    }
}

uint16_t datamgr_get_room_id(sensor_id_t sensor_id){
    int j = 0;

    if(roomAndSensor == NULL){
        printf("List is not initialized");
        exit(EXIT_FAILURE);
    }


    dplist_node_t* traveller = dpl_get_reference_at_index(roomAndSensor,j); // getting the head of the list
        while(traveller != NULL){
            listElement *tempElement = dpl_get_element_at_reference(roomAndSensor, traveller);
            if (tempElement != NULL && tempElement->sensorId == sensor_id) {
                return tempElement->roomId;
            }
            j++;
            traveller = dpl_get_reference_at_index(roomAndSensor,j);
        }

    return 0;

}


sensor_value_t datamgr_get_avg(sensor_id_t sensor_id){
    int j = 0;

    if(roomAndSensor == NULL){
        printf("List is not initialized");
        exit(EXIT_FAILURE);
    }

    dplist_node_t* traveller = dpl_get_reference_at_index(roomAndSensor,j); // getting the head of the list
    while(traveller != NULL){
        listElement *tempElement = dpl_get_element_at_reference(roomAndSensor, traveller);
        if (tempElement != NULL && tempElement->sensorId == sensor_id) {
            printf("Sensor %d final running avg: %.2f\n",tempElement->sensorId,tempElement->runningAvg);
            return tempElement->runningAvg;
        }
        j++;
        traveller = dpl_get_reference_at_index(roomAndSensor,j);
    }

    return 0;
}

time_t datamgr_get_last_modified(sensor_id_t sensor_id){
    int j = 0;

    if(roomAndSensor == NULL){
        printf("List is not initialized");
        exit(EXIT_FAILURE);
    }


    dplist_node_t* traveller = dpl_get_reference_at_index(roomAndSensor,j); // getting the head of the list
    while(traveller != NULL){
        listElement *tempElement = dpl_get_element_at_reference(roomAndSensor, traveller);
        if (tempElement != NULL && tempElement->sensorId == sensor_id) {
            return tempElement->lastModified;
        }
        j++;
        traveller = dpl_get_reference_at_index(roomAndSensor,j);
    }

    return 0;

}

int datamgr_get_total_sensors(){

    if(roomAndSensor == NULL){
        printf("List is not initialized");
        exit(EXIT_FAILURE);
    }
    printf("Total Nr of Sensors: %d\n", dpl_size(roomAndSensor));
    return dpl_size(roomAndSensor);
}

int existing_sensor_in_room(sensor_id_t sensorId) {
    if (roomAndSensor == NULL) {
        return 0;
    }

    for (int i = 0; i < dpl_size(roomAndSensor); i++) {
        dplist_node_t *currentNode = dpl_get_reference_at_index(roomAndSensor, i);
        listElement *currentElement = dpl_get_element_at_reference(roomAndSensor, currentNode);
        if (currentElement != NULL && currentElement->sensorId == sensorId) {
            return 1;
        }
    }

    return 0;
}

int existing_sensor_in_buffer(sensor_id_t sensorId){
    if (binaryDataList == NULL) {
        return 0;
    }

    for (int i = 0; i < dpl_size(binaryDataList); i++) {
        dplist_node_t *currentNode = dpl_get_reference_at_index(binaryDataList, i);
        listElement *currentElement = dpl_get_element_at_reference(binaryDataList, currentNode);
        if (currentElement != NULL && currentElement->sensorId == sensorId) {
            return 1;
        }
    }

    return 0;
}

void update_running_avg(sensor_id_t sensorId, sensor_value_t newTemp) {
    listElement *roomElement = NULL;
    // Find the roomElement corresponding to sensorId
    for (int k = 0; k < dpl_size(roomAndSensor); k++) {
        roomElement = dpl_get_element_at_reference(roomAndSensor, dpl_get_reference_at_index(roomAndSensor, k));
        if (roomElement->sensorId == sensorId) break;
    }
    if (roomElement == NULL) return; // Sensor not found

    // Add new temperature to the sum
    roomElement->lastFiveSum += newTemp;
    if (roomElement->lastFiveCount < RUN_AVG_LENGTH) {
        roomElement->lastFiveCount++;
    } else {
        // If we have reached the maximum number of elements, adjust the sum to remove the oldest element
        roomElement->lastFiveSum -= roomElement->lastFiveSum / roomElement->lastFiveCount;
    }

    // Calculate the running average
    roomElement->runningAvg = roomElement->lastFiveSum / roomElement->lastFiveCount;

    // Check and log based on the updated running average temperature
    char message[100];
    if (roomElement->runningAvg > SET_MAX_TEMP) {
        snprintf(message, sizeof(message), "Sensor %d reports that it's too hot! (avg temp = %0.2f)\n", roomElement->sensorId, roomElement->runningAvg);
        write_to_log_process(message);
    } else if (roomElement->runningAvg < SET_MIN_TEMP) {
        snprintf(message, sizeof(message), "Sensor %d reports that it's too cold! (avg temp = %0.2f)\n", roomElement->sensorId, roomElement->runningAvg);
        write_to_log_process(message);
    }
}


