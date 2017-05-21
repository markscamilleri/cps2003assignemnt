/**
 * @file
 * @author Mark Said Camilleri
 * @version 20170521.
 */

#include "network_client.h"

void init_client(char *host) {
    struct sockaddr_in serv_addr;
    struct hostent *server;

    //Getting socket descriptor
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        ZF_LOGF_STR("ERROR opening socket");
        exit(1);
    }

    //Resolving host name
    server = gethostbyname(host);
    if (server == NULL) {
        ZF_LOGW_STR("ERROR, no such host");
        exit(1);
    }

    //Sets first n bytes of the area to zero
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr,
          (char *) &serv_addr.sin_addr.s_addr,
          server->h_length);

    //Converting unsigned short integer from host byte order to network byte order.
    serv_addr.sin_port = htons(PORT_NUMBER);

    //Attempt connection with server
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        ZF_LOGF_STR("ERROR connecting");
        exit(1);
    }
}

void send_message_to_server(char *message) {
    //Send message to server
    ssize_t n = write(sockfd, message, strlen(message));
    if (n < 0)
        ZF_LOGW_STR("ERROR writing to socket");
}

void receive_message() {
    //Receive message from server
    char buffer[255];

    ssize_t n = read(sockfd, buffer, 255);
    if (n < 0)
        ZF_LOGW_STR("ERROR reading from socket");
    printf("%s\n", buffer);
}

void close_connection() {
    close(sockfd);
}