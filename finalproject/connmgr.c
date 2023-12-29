//
// Created by dony on 21/12/23.
//
/**
 * \author {Dony}
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "config.h"
#include "lib/tcpsock.h"
#include <pthread.h>
#include <stdbool.h>
#include "sbuffer.h"

sbuffer_t *tempBuffer;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;


void* handle_client(void* arg) {
    tcpsock_t *client = (tcpsock_t *)arg;
    sensor_data_t data;
    int bytes, result;
    char message[100];
    bool isFirstData = true; // flag to check if this is the first data received

    do {
        // read sensor ID
        bytes = sizeof(data.id);
        result = tcp_receive(client, (void *) &data.id, &bytes);
        // read temperature
        bytes = sizeof(data.value);
        result = tcp_receive(client, (void *) &data.value, &bytes);
        // read timestamp
        bytes = sizeof(data.ts);
        result = tcp_receive(client, (void *) &data.ts, &bytes);

        if (isFirstData && (result == TCP_NO_ERROR) && bytes) {
            sprintf(message, "Sensor %hu has opened a connection\n", data.id);
            write_to_log_process(message);
            isFirstData = false; // Set flag to false after logging the first data
        }

        pthread_mutex_lock(&mutex2);
        if ((result == TCP_NO_ERROR) && bytes) {
            printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", data.id, data.value,
                   (long int) data.ts);
            sbuffer_insert(tempBuffer, &data);

            printf("Data inserted into buffer\n");
        }
        pthread_mutex_unlock(&mutex2);
    } while (result == TCP_NO_ERROR);

    if (result == TCP_CONNECTION_CLOSED) {
        char signOff[56];
        sprintf(signOff,"Peer with sensor ID %hu has closed connection\n", data.id);
        write_to_log_process(signOff);
    }
    else {
        printf("Error occurred on connection to peer\n");
    }
    tcp_close(&client);

    pthread_exit(EXIT_SUCCESS);
}


int connection_manager(int PORT, int MAX_CONN, sbuffer_t* sbuffer) {
    tcpsock_t *server, *client;
    int conn_counter = 0;
    pthread_t thread[MAX_CONN];
    tempBuffer = sbuffer;

    if (tcp_passive_open(&server, PORT) != TCP_NO_ERROR) exit(EXIT_FAILURE);
    printf("Test server is started\n");

    while (1) {
        if (conn_counter >= MAX_CONN) {
            break;
        }

        if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR) break;
        printf("Incoming client connection\n");

        if (pthread_create(&thread[conn_counter], NULL, handle_client, client) != 0) {
            printf("Failed to create thread\n");
            break;
        }

        conn_counter++;
    }

    for (int i = 0; i < conn_counter; ++i) {
        if (pthread_join(thread[i], NULL) != 0) {
            fprintf(stderr, "Error joining thread %d\n", i);
        }
    }


    if (tcp_close(&server) != TCP_NO_ERROR) exit(EXIT_FAILURE);
    printf("Test server is shutting down\n");

    sensor_data_t dummy;
    dummy.id = 0;
    dummy.value=0;
    dummy.ts=0;
    sbuffer_insert(sbuffer,&dummy);

    return 0;
}