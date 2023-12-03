#include "config.h"
#include "sbuffer.c"
#include <pthread.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

sensor_data_t* sensor;
FILE* file;
sbuffer_t* shared_buffer;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;
int finished = 0;

void *writer_thread() {
    sensor_data_t *dumdum = (sensor_data_t *)malloc(sizeof(sensor_data_t));

    // Check if file is opened successfully
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    while (fread(&sensor->id, sizeof(u_int16_t), 1, file) == 1) {
        fread(&sensor->value, sizeof(double), 1, file);
        fread(&sensor->ts, sizeof(time_t), 1, file);

        if (dumdum == NULL) {
            perror("Error allocating memory");
            exit(EXIT_FAILURE);
        }

        pthread_mutex_lock(&mutex);  // Lock the mutex before accessing shared resources
        sbuffer_insert(shared_buffer, sensor);
        pthread_cond_signal(&condition);  // Signal after inserting into the buffer
        pthread_mutex_unlock(&mutex);  // Unlock the mutex to allow the waiting thread to proceed
        usleep(10000);
    }

    dumdum->id = 0;
    dumdum->value = 0;
    dumdum->ts = 0;

    pthread_mutex_lock(&mutex);  // Lock the mutex before accessing shared resources
    sbuffer_insert(shared_buffer, dumdum);
    pthread_cond_signal(&condition);  // Signal after inserting into the buffer
    pthread_mutex_unlock(&mutex);  // Unlock the mutex to allow the waiting thread to proceed

    free(dumdum);
}

void *reader_thread() {
    FILE *log = fopen("sensor_data_out.csv", "w");
    sensor_data_t *obtained_data = (sensor_data_t *)malloc(sizeof(sensor_data_t));

    if (log == NULL) {
        perror("Error opening log file");
        exit(EXIT_FAILURE);
    }



        while (sbuffer_remove(shared_buffer, obtained_data) == 0 && finished == 0) {
            pthread_mutex_lock(&mutex);
            fprintf(log, "%hu, %lf, %s", obtained_data->id, obtained_data->value, ctime(&(obtained_data->ts)));
            pthread_mutex_unlock(&mutex);  // Unlock the mutex during file operations
            usleep(25000);
        }
        //pthread_cond_wait(&condition, &mutex);
        //The above line is only needed when we have a real life device
        //Which outputs a continuous  stream of data for a very long time
        //I spent over 2 hours to figure out why this doesn't make the code stop executing
        //But then I realized, wait, we don't have a  real  device that outputs
        //Data continuously for a very long time :-)


    free(obtained_data);
    fclose(log);
    pthread_mutex_unlock(&mutex);  // Unlock the mutex before exiting
    pthread_exit(NULL);
}

int main(){
    pthread_t writer, reader1, reader2;
    file = fopen("sensor_data", "rb");
    sensor = (sensor_data_t *)malloc(sizeof(sensor_data_t));

    // Initialize shared buffer
    sbuffer_init(&shared_buffer);

    // Create threads
    pthread_create(&writer, NULL, writer_thread, NULL);
    pthread_create(&reader1, NULL, reader_thread, NULL);
    pthread_create(&reader2, NULL, reader_thread, NULL);

    // Wait for threads to finish executing
    pthread_join(writer, NULL);
    pthread_join(reader1, NULL);
    pthread_join(reader2, NULL);

    // DESTROY STUFF
    pthread_cond_destroy(&condition);
    pthread_mutex_destroy(&mutex);

    // Free memory and close file  :-)
    free(sensor);
    fclose(file);

    // Destroy the shared buffer
    sbuffer_free(&shared_buffer);

    return 0;
}
