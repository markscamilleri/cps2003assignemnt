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

int sockfd;

void init_server(void);

void send_message_to_server(char *message);

void receive_message();

void close_connection();


#endif //ASSIGNMENT_NETWORK_CLIENT_H
