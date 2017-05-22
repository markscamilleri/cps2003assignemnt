/**
 * @file
 * @author Mark Said Camilleri
 * @version 20170521.
 */

#ifndef ASSIGNMENT_NETWORK_CLIENT_H
#define ASSIGNMENT_NETWORK_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "../config.h"
#include "../../deps/zf_log/zf_log.h"

int closeClient;

void init_client(char *host);

void send_message_to_server(char *message);

void receive_message(void);

void close_connection(void);

void close_client(void);


#endif //ASSIGNMENT_NETWORK_CLIENT_H
