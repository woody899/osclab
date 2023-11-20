//
// Created by dony on 18/11/23.
//
#include <stdio.h>
#include <stdbool.h>
#include "config.h"




FILE * open_db(char * filename, bool append){
    if(filename == NULL){
        perror("No file name entered?");
    }
    if(append){
        FILE* file = fopen(filename,"a");
        return file;
    } else{
        FILE* file = fopen(filename,"w");
        return file;
    }
}

int insert_sensor(FILE * f, sensor_id_t id, sensor_value_t value, sensor_ts_t ts){
    if(f == NULL){
        perror("File not found");
    } else {
        fprintf(f, "%hu, %f, %ld\n", id, value, ts);
    }
    return 0;
}

int close_db(FILE * f){
    fclose(f);
    return 0;
}

