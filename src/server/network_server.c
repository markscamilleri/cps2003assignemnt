/**
 * @file
 * @author Mark Said Camilleri
 * @version 20170521.
 */

#ifndef ASSIGNMENT_NETWORK_SERVER_C
#define ASSIGNMENT_NETWORK_SERVER_C

#include "network_server.h"

ListNode *connectionList = NULL;
pthread_mutex_t connectionListMutex = PTHREAD_MUTEX_INITIALIZER;

int server_sockfd;

/// Set to true if server is being closed (to close gracefully and not accept connecitons)
int closeServer = 0;

void init_server_and_accept_connections(int *start_sending) {
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    //Creates an endpoint for communication and returns a descriptor
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd < 0) {
        ZF_LOGF_STR("ERROR opening socket");
        exit(EXIT_FAILURE);
    }

    //Sets first n bytes of the address to zero
    bzero((char *) &serv_addr, sizeof(serv_addr));

    //Converting initial portion of string to integer
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    //Converting unsigned short integer from host byte order to network byte order.
    serv_addr.sin_port = htons(PORT_NUMBER);

    //Assigning address specified by addr to the socket referred by the server_sockfd
    if (bind(server_sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0) {
        ZF_LOGF_STR("ERROR on binding");
        close(server_sockfd);
        exit(EXIT_FAILURE);
    }

    //Marking socket as a socket that will be used to accept incoming connection requests
    ZF_LOGI_STR("Listening for connections");
    listen(server_sockfd, 5);
    clilen = sizeof(cli_addr);

    *start_sending = 1;

    //Accepting an incoming connection request
    while (!closeServer) {
        aceept_connection((struct sockaddr *) &cli_addr, &clilen);
    }

    pthread_exit(NULL);
}

void aceept_connection(struct sockaddr *cli_addr, socklen_t *clilen) {
    int newsockfd = accept(server_sockfd, cli_addr, clilen);
    if (newsockfd < 0)
        ZF_LOGW_STR("ERROR on accepting connection request");
    else {

        if (connectionList == NULL) {
            ZF_LOGI("Allocating memory");
            connectionList = malloc(sizeof(ListNode));
            ZF_LOGI("Putting data");
            connectionList->newsockfd = newsockfd;
            connectionList->next = NULL;
            ZF_LOGI("Data is successful");
            ZF_LOGI("%d", connectionList->newsockfd);
        } else ListNode_add(connectionList, newsockfd);
        ZF_LOGI("There are %d nodes in the list", ListNode_size(connectionList));
    }
}

void close_all_connections(ListNode *node) {
    if (node != NULL) {
        close(node->newsockfd);
        close_all_connections(node->next);
    }
}

void close_all(void) {
    broadcast_message(SERVER_DOWN);
    close(server_sockfd);
    //Lock connection list
    pthread_mutex_lock(&connectionListMutex);

    close_all_connections(connectionList);
    ListNode_clear(connectionList);

    //Unlock and destroy connection list
    pthread_mutex_unlock(&connectionListMutex);
    pthread_mutex_destroy(&connectionListMutex);
}

void send_message_to_client(ListNode *node, char *message) {
    size_t size = strlen(message);

    write(node->newsockfd, &size, sizeof(size_t));
    write(node->newsockfd, message, size);
}

void send_message_to_list(ListNode *node, char *message) {
    if (node != NULL) {
        send_message_to_client(node, message);
        send_message_to_list(node->next, message);
    }
}

void broadcast_message(char *message) {
    ZF_LOGI_STR("Message Sending");
    ZF_LOGI("Sending %s", message);
    //LOCK MUTEX
    send_message_to_list(connectionList, message);
    //UNLOCK MUTEX
    ZF_LOGI_STR("Message Sent");
}

void close_server() {
    closeServer = 1;
    close_all();
}

ListNode *get_connection_list() {
    ZF_LOGI("Pointer sent: %p", connectionList);
    return connectionList;
}

#endif  // ASSIGNMENT_NETWORK_SERVER_C