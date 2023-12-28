//
// Created by dony on 21/12/23.
//

#ifndef FINALPROJECT_CONNMGR_H
#define FINALPROJECT_CONNMGR_H

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "config.h"
#include "sbuffer.h"
#include "lib/tcpsock.h"
#include <pthread.h>

/*
 * Handles the specific task of what each client does
 * Each client writes to the shared buffer
 */
void* handle_client(void* arg);

/*
 * Manages the connection between several clients
 * and the threads of those clients as well.
 */
int connection_manager(int PORT, int MAX_CONN, sbuffer_t* sbuffer);

#endif //FINALPROJECT_CONNMGR_H
