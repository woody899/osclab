/**
 * \author {AUTHOR}
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "sbuffer.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;


/**
 * basic node for the buffer, these nodes are linked together to create the buffer
 */
typedef struct sbuffer_node {
    struct sbuffer_node *next;  /**< a pointer to the next node*/
    sensor_data_t data;         /**< a structure containing the data */
    int mgrStorage;
    int mgrData;
} sbuffer_node_t;

/**
 * a structure to keep track of the buffer
 */
struct sbuffer {
    sbuffer_node_t *head;       /**< a pointer to the first node in the buffer */
    sbuffer_node_t *tail;       /**< a pointer to the last node in the buffer */
};

int sbuffer_init(sbuffer_t **buffer) {
    *buffer = malloc(sizeof(sbuffer_t));
    if (*buffer == NULL) return SBUFFER_FAILURE;
    (*buffer)->head = NULL;
    (*buffer)->tail = NULL;
    return SBUFFER_SUCCESS;
}

int sbuffer_free(sbuffer_t **buffer) {
    sbuffer_node_t *dummy;
    if ((buffer == NULL) || (*buffer == NULL)) {
        return SBUFFER_FAILURE;
    }
    while ((*buffer)->head) {
        dummy = (*buffer)->head;
        (*buffer)->head = (*buffer)->head->next;
        free(dummy);
    }
    free(*buffer);
    *buffer = NULL;

    //destroy the mutex and the condition variable once free is called in the end
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condition);

    return SBUFFER_SUCCESS;
}

int sbuffer_remove(sbuffer_t *buffer, sensor_data_t *data) {
    pthread_mutex_lock(&mutex);
    sbuffer_node_t *dummy;
    if (buffer == NULL) return SBUFFER_FAILURE;
    while (buffer->head == NULL) {
        pthread_cond_wait(&condition,&mutex);
    }
    *data = buffer->head->data;
    dummy = buffer->head;
    if(data->id == 0) {
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&condition);
        return SBUFFER_NO_DATA;
    }

    if(buffer->head->mgrData == 1 && buffer->head->mgrStorage == 1) {
        if (buffer->head == buffer->tail) // buffer has only one node
        {
            buffer->head = buffer->tail = NULL;
        } else  // buffer has many nodes empty
        {
            buffer->head = buffer->head->next;
        }
    }
    free(dummy);
    pthread_mutex_unlock(&mutex);
    return SBUFFER_SUCCESS;
}

int sbuffer_insert(sbuffer_t *buffer, sensor_data_t *data) {
    pthread_mutex_lock(&mutex);
    sbuffer_node_t *dummy;
    if (buffer == NULL) return SBUFFER_FAILURE;
    dummy = malloc(sizeof(sbuffer_node_t));
    if (dummy == NULL) return SBUFFER_FAILURE;
    dummy->data = *data;
    dummy->next = NULL;
    dummy->mgrStorage = 0;
    dummy->mgrData = 0;
    if (buffer->tail == NULL) // buffer empty (buffer->head should also be NULL
    {
        buffer->head = buffer->tail = dummy;
    } else // buffer not empty
    {
        buffer->tail->next = dummy;
        buffer->tail = buffer->tail->next;
    }
    pthread_cond_signal(&condition);
    pthread_mutex_unlock(&mutex);

    return SBUFFER_SUCCESS;
}


int sbuffer_read(sbuffer_t *buffer, sensor_data_t *data) {
    pthread_mutex_lock(&mutex);
    if (buffer == NULL || data == NULL) return SBUFFER_FAILURE;
    while (buffer->head == NULL) {
        pthread_cond_wait(&condition, &mutex);
    }
    *data = buffer->head->data;
    if (data->id == 0) {
        pthread_mutex_unlock(&mutex);
        return SBUFFER_NO_DATA;
    }
    pthread_mutex_unlock(&mutex);
    return SBUFFER_SUCCESS;
}

int sbuffer_update_mgrData(sbuffer_t *buffer) {
    pthread_mutex_lock(&mutex);
    if (buffer == NULL || buffer->head == NULL) {
        pthread_mutex_unlock(&mutex);
        return SBUFFER_FAILURE;
    }

    buffer->head->mgrData = 1;

    pthread_mutex_unlock(&mutex);
    return SBUFFER_SUCCESS;
}


int sbuffer_update_mgrStorage(sbuffer_t *buffer){
    pthread_mutex_lock(&mutex);
    if (buffer == NULL || buffer->head == NULL) {
        pthread_mutex_unlock(&mutex);
        return SBUFFER_FAILURE;
    }

    buffer->head->mgrStorage = 1;

    pthread_mutex_unlock(&mutex);
    return SBUFFER_SUCCESS;
}


int sbuffer_getFlagData(sbuffer_t *sbuffer,sensor_id_t sensorId){
    sbuffer_node_t *current = sbuffer->head;
    while (current != NULL) {
        if (current->data.id == sensorId) {
            return current->mgrData;
        }
        current = current->next;
    }

    return -1;

}

int sbuffer_getFlagStorage(sbuffer_t *sbuffer,sensor_id_t sensorId){
    sbuffer_node_t *current = sbuffer->head;
    while (current != NULL) {
        if (current->data.id == sensorId) {
            return current->mgrStorage;
        }
        current = current->next;
    }
    return -1;
}

