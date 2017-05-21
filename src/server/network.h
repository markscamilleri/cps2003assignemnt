/**
 * @file
 * @author Mark Said Camilleri
 * @version 20170521
 */

#ifndef SERVER_NETWORK_H
#define SERVER_NETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "listnode.h"
#include "../../deps/zf_log/zf_log.h"

#define PORT_NUMBER 123456

ListNode * connectionList = NULL;

int sockfd;

void init(void);

void acceptConnections(struct sockaddr * cli_addr, socklen_t *clilen);

void close_all_connections(ListNode * node);

void close_all(void);

void send_message(ListNode * node, char* message);

void send_message_to_list(ListNode * node, char* message);

void broadcast_message(char * message);

#endif  //SERVER_NETWORK_H
