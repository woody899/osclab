/**
 * \author {Woody}
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "config.h"
#include "lib/tcpsock.h"
#include <pthread.h>



void* handle_client(void* arg){
    tcpsock_t *client = (tcpsock_t *)arg;
    sensor_data_t data;
    int bytes, result;

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
            if ((result == TCP_NO_ERROR) && bytes) {
                printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", data.id, data.value,
                       (long int) data.ts);
            }
        } while (result == TCP_NO_ERROR);
        if (result == TCP_CONNECTION_CLOSED) {
            printf("Peer has closed connection\n");

        }
        else {
            printf("Error occurred on connection to peer\n");
        }
        tcp_close(&client);

    pthread_exit(EXIT_SUCCESS);
}


int main(int argc, char *argv[]) {
    tcpsock_t *server, *client;
    int MAX_CONN = atoi(argv[2]);
    int PORT = atoi(argv[1]);
    int conn_counter = 0;
    pthread_t thread[MAX_CONN];

    if(argc < 3) {
    	printf("Please provide the right arguments: first the port, then the max nb of clients");
    	return -1;
    }


    if (tcp_passive_open(&server, PORT) != TCP_NO_ERROR) exit(EXIT_FAILURE);
    printf("Test server is started\n");

    while (1) {
        if(conn_counter >= MAX_CONN){
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
    return 0;
}