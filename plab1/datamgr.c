#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <assert.h>
#include "config.h"
#include "lib/dplist.h"

typedef struct {
    u_int16_t roomId;
    u_int16_t sensorId;
    double runningAvg;
    time_t lastModified;
} list_element;

void *element_copy(void *element)
{
    list_element *copy = malloc(sizeof(list_element));
    assert(copy != NULL);
    copy->roomId = ((list_element *)element)->roomId;
    copy->sensorId = ((list_element *)element)->sensorId;
    copy->runningAvg = ((list_element *)element)->runningAvg;
    copy->lastModified = ((list_element *)element)->lastModified;
    return (void *)copy;
}

void element_free(void **element)
{
    free(*element);
    *element = NULL;
}

int element_compare(void *x, void *y)
{
    return ((((list_element *)x)->roomId < ((list_element *)y)->roomId) ? -1 : (((list_element *)x)->roomId == ((list_element *)y)->roomId) ? 0 : 1);
}

dplist_t *sensorStuff = NULL;

void datamgr_parse_sensor_files(FILE *fp_sensor_map, FILE *fp_sensor_data)
{
    // Parsing MAP files
    uint16_t num1, num2;
    int count = 0;
    list_element *element = malloc(sizeof(list_element)); // Allocate memory for list_element
    if (element == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }

    while (fscanf(fp_sensor_map, "%hu %hu", &num1, &num2) == 2)
    {
        printf("Room id: %hu and Sensor id: %hu\n", num1, num2);
        element->roomId = num1;
        element->sensorId = num2;
        // Add the element to the list
        sensorStuff = dpl_insert_at_index(sensorStuff, element, count, true);
        count++;
    }

    // Parsing binary data

    while(fread(fp_sensor_data,sizeof(sensor_data_t),1,))


}

//Just removing what is in the linked list.
void datamgr_free(){
    dpl_free(&sensorStuff,true);
}
