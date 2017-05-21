
#include "network_server.h"

void init_server(void) {
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    //Creates an endpoint for communication and returns a descriptor
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        ZF_LOGF_STR("ERROR opening socket");
        exit(1);
    }

    //Sets first n bytes of the area to zero
    bzero((char *) &serv_addr, sizeof(serv_addr));

    //Converting initial portion of string to integer
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    //Converting unsigned short integer from host byte order to network byte order.
    serv_addr.sin_port = htons(PORT_NUMBER);

    //Assigning address specified by addr to the socket referred by the sockfd
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0) {
        ZF_LOGF_STR("ERROR on binding");
        exit(1);
    }

    //Marking socket as a socket that will be used to accept incoming connection requests
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    //Accepting an incoming connection request
    // TODO SPLIT-THREAD
    acceptConnections((struct sockaddr *) &cli_addr, &clilen);

    //TODO split again and accept messages (and parse them)
}

void acceptConnections(struct sockaddr *cli_addr, socklen_t *clilen) {
    int newsockfd = accept(sockfd, cli_addr, clilen);
    if (newsockfd < 0)
        ZF_LOGW_STR("ERROR on accepting connection request");
    else ListNode_add(connectionList, newsockfd);
}

void close_all_connections(ListNode *node) {
    if (node != NULL) {
        close(node->newsockfd);
        close_all_connections(node->next);
    }
}

void close_all(void) {
    close(sockfd);
    close_all_connections(connectionList);
    ListNode_clear(connectionList);
}

void send_message_to_client(ListNode *node, char *message) {
    write(node->newsockfd, message, sizeof(message)/sizeof(char));
}

void send_message_to_list(ListNode *node, char *message) {
    if (node != NULL) {
        send_message_to_client(node, message);
        send_message_to_list(node->next, message);
    }
}

void broadcast_message(char *message) {
    send_message_to_list(connectionList, message);
}