/**
 * \author {AUTHOR}
 */

#ifndef _SBUFFER_H_
#define _SBUFFER_H_

#include "config.h"

#define SBUFFER_FAILURE -1
#define SBUFFER_SUCCESS 0
#define SBUFFER_NO_DATA 1

typedef struct sbuffer sbuffer_t;
/**
 * basic node for the buffer, these nodes are linked together to create the buffer
 */
typedef struct sbuffer_node {
    struct sbuffer_node *next;  /**< a pointer to the next node*/
    sensor_data_t data;         /**< a structure containing the data */

} sbuffer_node_t;

/**
 * a structure to keep track of the buffer
 */
struct sbuffer {
    sbuffer_node_t *head;       /**< a pointer to the first node in the buffer */
    sbuffer_node_t *tail;       /**< a pointer to the last node in the buffer */
};

/**
 * Allocates and initializes a new shared buffer
 * \param buffer a double pointer to the buffer that needs to be initialized
 * \return SBUFFER_SUCCESS on success and SBUFFER_FAILURE if an error occurred
 */
int sbuffer_init(sbuffer_t **buffer);

/**
 * All allocated resources are freed and cleaned up
 * \param buffer a double pointer to the buffer that needs to be freed
 * \return SBUFFER_SUCCESS on success and SBUFFER_FAILURE if an error occurred
 */
int sbuffer_free(sbuffer_t **buffer);

/**
 * Removes the first sensor data in 'buffer' (at the 'head') and returns this sensor data as '*data'
 * If 'buffer' is empty, the function doesn't block until new sensor data becomes available but returns SBUFFER_NO_DATA
 * \param buffer a pointer to the buffer that is used
 * \param data a pointer to pre-allocated sensor_data_t space, the data will be copied into this structure. No new memory is allocated for 'data' in this function.
 * \return SBUFFER_SUCCESS on success and SBUFFER_FAILURE if an error occurred
 */
int sbuffer_remove(sbuffer_t *buffer, sensor_data_t *data);

/**
 * Inserts the sensor data in 'data' at the end of 'buffer' (at the 'tail')
 * \param buffer a pointer to the buffer that is used
 * \param data a pointer to sensor_data_t data, that will be copied into the buffer
 * \return SBUFFER_SUCCESS on success and SBUFFER_FAILURE if an error occured
*/
int sbuffer_insert(sbuffer_t *buffer, sensor_data_t *data);


/**
 * This basically reads the sbuffer without removing the data from the buffer
 */
int sbuffer_read(sbuffer_t *buffer, sensor_data_t *data);

/**
 * @param buffer a pointer to the buffer that is used
 * Updates the flag which indicates whether the data manager has read from the buffer or not.
 */
int sbuffer_update_mgrData(sbuffer_t *buffer);

/**
 *
 * @param buffer a pointer to the buffer that is used
 * @return the value of the storage manager flag
 */
int sbuffer_update_mgrStorage(sbuffer_t *buffer);

/**
 *
 * @param sbuffer
 * @param sensorId
 * @return
 */
int sbuffer_getFlagData(sbuffer_t *sbuffer,sensor_id_t sensorId);

/**
 *
 * @param sbuffer
 * @param sensorId
 * @return
 */
int sbuffer_getFlagStorage(sbuffer_t *sbuffer,sensor_id_t sensorId);

#endif  //_SBUFFER_H_
