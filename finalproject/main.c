//
// Created by dony on 21/12/23.
//
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <pthread.h>
#include "sbuffer.h"
#include "connmgr.h"
#include "datamgr.h"
#include "sensor_db.h"


pid_t id;
int logging_fd[2];
char mysteriousBuffer[100000];
int i = 0;
int MAX_CONN;
int PORT;
sbuffer_t* sharedBuffer;
pthread_mutex_t main_mutex = PTHREAD_MUTEX_INITIALIZER;


void *connectionPasser(){
    connection_manager(PORT,MAX_CONN,sharedBuffer);
    pthread_exit(NULL);
}


void *storagePasser(){
    FILE* fPointer = open_db("data.csv",true);
    sensor_data_t* temp = (sensor_data_t*) malloc(sizeof(sensor_data_t));
    if(fPointer == NULL){
        printf("Error in opening CSV file");
        exit(EXIT_FAILURE);
    }

    while(sbuffer_remove(sharedBuffer,temp) == 0){

        //pthread_mutex_lock(&main_mutex);

        if(temp->mgrStorage == 0) {
            // fprintf(fpointer,"%hu, %f, %ld\n",temp->id,temp->value,temp->ts); <- this will still work
            // however, I think it would be wise to use insert_sensor instead, otherwise we end up with an
            //unused function. Besides, it already has a write_to_log process called in it.
            insert_sensor(fPointer, temp->id, temp->value, temp->ts);
            printf("data inserted into csv file\n");
            sharedBuffer->head->data.mgrStorage = 1;
        }
        fflush(fPointer);
        //pthread_mutex_unlock(&main_mutex);
    }
    free(temp);
    close_db(fPointer);
    pthread_exit(EXIT_SUCCESS);
}

void *dataPasser(){
    FILE* map = fopen("room_sensor.map","r");

    datamgr_parse_sensor_files(map,sharedBuffer, &main_mutex);

    fclose(map);
    pthread_exit(EXIT_SUCCESS);
}



int write_to_log_process(char *msg) {
    write(logging_fd[1], msg, strlen(msg));
    return 0;
}

int end_log_process(){
    close(logging_fd[1]);
    waitpid(id,NULL,0);
    return 0;
}


int main(int argc, char* argv[]){
    sbuffer_init(&sharedBuffer);
    MAX_CONN = atoi(argv[2]);
    PORT = atoi(argv[1]);

    if (argc < 3) {
        printf("Please provide the right arguments: first the port, then the max nb of clients");
        return -1;
    }


    if (pipe(logging_fd) == -1) {
        perror("PIPE FAILED");
        return -1;
    }

    id = fork();

    if (id == -1) {
        perror("FORK FAILED");
        return -1;
    }

    //initialize the three major threads
    pthread_t connectionThread;
    pthread_t dataMgrThread;
    pthread_t storageThread;

    if(id > 0) {
        //PARENT PROCESS

        //create the threads
        pthread_create(&connectionThread, NULL, connectionPasser, NULL);
        pthread_create(&storageThread,NULL,storagePasser,NULL);
        pthread_create(&dataMgrThread,NULL,dataPasser,NULL);

        //Join the threads

        pthread_join(connectionThread, NULL);
        pthread_join(storageThread,NULL);
        pthread_join(dataMgrThread,NULL);

        pthread_mutex_destroy(&main_mutex);

        sbuffer_free(&sharedBuffer);
        pthread_exit(NULL);

    }

    if(id == 0){

        FILE *log = fopen("log_gateway.log", "a");
        close(logging_fd[1]);
        while ((read(logging_fd[0], mysteriousBuffer, sizeof(mysteriousBuffer))) > 0) {
            char *event = strtok(mysteriousBuffer, "\n");
            while (event != NULL) {
                 time_t mytime = time(NULL);
                 char *time_str = ctime(&mytime);
                 time_str[strlen(time_str) - 1] = '\0';
                 fprintf(log, "%d - %s %s\n", i, time_str, event);
                 fflush(log);
                 i++;
                 event = strtok(NULL, "\n");
            }
            //memset basically resets the buffer
            memset(mysteriousBuffer,0,sizeof(mysteriousBuffer));
        }
        fclose(log);
        close(logging_fd[0]);  // Close the read end of the logging pipe
        exit(0);
    }
    return 0;
}
