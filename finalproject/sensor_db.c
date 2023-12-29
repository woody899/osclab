//
// Created by dony on 18/11/23.
//
#include <stdio.h>
#include <stdbool.h>
#include "config.h"
#include "datamgr.h"



FILE * open_db(char * filename, bool append){
    if(filename == NULL){
        perror("No file name entered?");
        exit(EXIT_FAILURE);
    }
    write_to_log_process("Data file opened\n");

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
        return -1;
    } else if(existing_sensor_in_room(id) != 1){
        char message[56];
        sprintf(message,"Received data from invalid sensor node ID %hu\n",id);
        write_to_log_process(message);
        return -1;
    }else {
        fprintf(f, "%hu, %f, %ld\n", id, value, ts);
    }
    write_to_log_process("Data inserted\n");

    return 0;
}

int close_db(FILE * f){
    if(f == NULL){
        perror("No file to close");
        return 0;
    }
    fclose(f);
    write_to_log_process("Data file closed\n");
    return 1;
}

