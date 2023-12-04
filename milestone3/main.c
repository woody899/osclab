#include "config.h"
#include "sbuffer.h"
#include <pthread.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

sensor_data_t* sensor;
FILE* file;
sbuffer_t* shared_buffer;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
//pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
void *writer_thread() {
    sensor_data_t *dumdum = (sensor_data_t *)malloc(sizeof(sensor_data_t));

    if (file == NULL) {
        perror("FILE NOT FOUND");
        return 0;
    }

    while (fread(&sensor->id, sizeof(u_int16_t), 1, file) == 1) {
        fread(&sensor->value, sizeof(double), 1, file);
        fread(&sensor->ts, sizeof(time_t), 1, file);



        sbuffer_insert(shared_buffer, sensor);
        usleep(10000);
    }

    dumdum->id = 0;
    dumdum->value = 0;
    dumdum->ts = 0;

    sbuffer_insert(shared_buffer, dumdum);
    free(dumdum);
    return NULL;

}

void *reader_thread() {
    FILE *log = fopen("sensor_data_out.csv", "a");
    sensor_data_t *obtained_data = (sensor_data_t *)malloc(sizeof(sensor_data_t));

    if (log == NULL) {
        perror("FILE NOT FOUND");
        return NULL;
    }

        while (sbuffer_remove(shared_buffer, obtained_data) != SBUFFER_NO_DATA) {
            pthread_mutex_lock(&mutex2);
            fprintf(log, "%hu, %lf, %s", obtained_data->id, obtained_data->value, ctime(&(obtained_data->ts)));
            fflush(log);
            pthread_mutex_unlock(&mutex2);
            usleep(25000);

        }
        //pthread_cond_wait(&cond2,&mutex2);
        //normally I would add a wait over here(after the while loop, like above), however, for this exercise
        // our data is not coming from a continuous stream and is confined to the
        // contents of the binary file. Obviously I would have to signal that condition from the
        // writer thread. 


    free(obtained_data);
    fclose(log);
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
    pthread_mutex_destroy(&mutex2);

    // Free memory and close file  :-)
    free(sensor);
    fclose(file);

    // remove and free the shared buffer
    sbuffer_free(&shared_buffer);

    return 0;
}
