#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <assert.h>
#include "config.h"
#include "lib/dplist.h"
#include "datamgr.h"
typedef struct {
    u_int16_t roomId;
    sensor_id_t sensorId;
    double runningAvg;
    time_t lastModified;
} listElement;



typedef struct {
    sensor_id_t sensorNo;
    sensor_value_t temperature;
    sensor_ts_t timestamp;
} sensorInfo;


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


void datamgr_parse_sensor_files(FILE *fp_sensor_map, FILE *fp_sensor_data) {
    if (!fp_sensor_map || !fp_sensor_data) {
        fprintf(stderr, "File pointers are NULL\n");
        return;
    }

    // Initialize list pointers
    roomAndSensor = dpl_create(element_copy, element_free, element_compare);
    binaryDataList = dpl_create(element_copy, element_free, element_compare);

    // Parsing MAP files
    uint16_t num1, num2;
    int count = 0;
    while (fscanf(fp_sensor_map, "%hu %hu", &num1, &num2) == 2) {
        listElement *element = malloc(sizeof(listElement));
        element->roomId = num1;
        element->sensorId = num2;
        roomAndSensor = dpl_insert_at_index(roomAndSensor, element, count, true);
        count++;
    }

    // Parsing binary data
    int i = 0;
    sensorInfo *sensorLog;
    while (true) {
        sensorLog = malloc(sizeof(sensorInfo));
        if (!sensorLog) {
            fprintf(stderr, "Memory allocation failed for sensorLog\n");
            break;
        }
        if (fread(&sensorLog->sensorNo, sizeof(u_int16_t), 1, fp_sensor_data) != 1) {
            free(sensorLog);
            break;
        }
        fread(&sensorLog->temperature, sizeof(double), 1, fp_sensor_data);
        fread(&sensorLog->timestamp, sizeof(time_t), 1, fp_sensor_data);

        // checking if sensorNo is in roomAndSensor list
        bool sensorExists = false;
        for(int k = 0; k < dpl_size(roomAndSensor); k++) {
            listElement *roomElement = dpl_get_element_at_reference(roomAndSensor, dpl_get_reference_at_index(roomAndSensor, k));
            if (roomElement->sensorId == sensorLog->sensorNo) {
                sensorExists = true;
                break;
            }
        }

        if (sensorExists) {
            dpl_insert_at_index(binaryDataList, sensorLog, i, true);
            i++;
        } else {
            printf("Sensor data with ID %hu not found in sensor map\n", sensorLog->sensorNo);
            free(sensorLog);
        }
    }

    sensor_value_t arrayAvg[5];
    int roomListSize = dpl_size(roomAndSensor);
    for(int k = 0; k < roomListSize; k++){
        listElement *roomElement = dpl_get_element_at_reference(roomAndSensor, dpl_get_reference_at_index(roomAndSensor, k));
        int tempCount = 0;
        double sum = 0;
        time_t latestTimestamp = 0;
        for (int j = 0; j < dpl_size(binaryDataList); ++j) {
            sensorInfo *binaryElement = dpl_get_element_at_reference(binaryDataList, dpl_get_reference_at_index(binaryDataList, j));
            if (binaryElement->timestamp > latestTimestamp) {
                latestTimestamp = binaryElement->timestamp;
            }
            if (roomElement->sensorId == binaryElement->sensorNo) {
                if (tempCount < 5) {
                    arrayAvg[tempCount] = binaryElement->temperature;
                    sum += arrayAvg[tempCount];
                    tempCount++;
                } else {
                    sum -= arrayAvg[0];
                    for (int x = 0; x < 4; ++x) {
                        arrayAvg[x] = arrayAvg[x + 1];
                    }
                    arrayAvg[4] = binaryElement->temperature;
                    sum += arrayAvg[4];
                }
            }
        }
        roomElement->runningAvg = (tempCount > 0) ? (sum / tempCount) : 0;
        roomElement->lastModified = latestTimestamp;
        k++;
    }
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
        perror("List not initialized");
        exit(EXIT_FAILURE);
    }


    dplist_node_t* traveller = dpl_get_reference_at_index(roomAndSensor,j); // getting the head of the list
        while(traveller != NULL){
            listElement *travellerElement = dpl_get_element_at_reference(roomAndSensor,traveller);
            if (travellerElement != NULL && travellerElement->sensorId == sensor_id) {
                return travellerElement->roomId;
            }
            j++;
            traveller = dpl_get_reference_at_index(roomAndSensor,j);
        }

    return 0;

}


//void printEverything(){
//    for(int i = 0; i < dpl_size(roomAndSensor);i++){
//        dplist_node_t* roomNode = dpl_get_reference_at_index(roomAndSensor,i);
//        listElement* elementRoom = dpl_get_element_at_reference(roomAndSensor,roomNode);
//        printf("Room ID: %hu, Sensor ID: %hu, RunningAvg: %f, Timestamp: %ld\n",elementRoom->roomId,elementRoom->sensorId,elementRoom->runningAvg,elementRoom->lastModified);
//    }
//
//   for(int k = 0; k < dpl_size(binaryDataList);k++){
//        dplist_node_t* nodeBinary = dpl_get_reference_at_index(binaryDataList,k);
//        sensorInfo* elementBinary = dpl_get_element_at_reference(binaryDataList,nodeBinary);
//        printf("Sensor Number: %hu, Timestamp: %ld, Temperature: %f \n",elementBinary->sensorNo,elementBinary->timestamp,elementBinary->temperature);
//    }
//}



