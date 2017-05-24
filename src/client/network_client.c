/**
 * @file
 * @author Mark Said Camilleri
 * @version 20170521.
 */

#include <pthread.h>
#include "network_client.h"

int client_sockfd;

void init_client(char *host) {
    closeClient = 0;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    //Getting socket descriptor
    client_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sockfd < 0) {
        ZF_LOGF_STR("ERROR opening socket");
        exit(EXIT_FAILURE);
    }

    atexit(close_client);

    //Resolving host name
    server = gethostbyname(host);
    if (server == NULL) {
        ZF_LOGW_STR("ERROR, no such host");
        exit(EXIT_FAILURE);
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
    if (connect(client_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        ZF_LOGF_STR("ERROR connecting");
        exit(EXIT_FAILURE);
    }

    atexit(close_client);

    pthread_t connectionThread;
    if (pthread_create(&connectionThread, NULL, receive_message, NULL) != 0) {
        ZF_LOGF_STR("Error when creating new thread to accept incoming messages");
        exit(EXIT_FAILURE);
    }
}

void send_message_to_server(char *message) {
    size_t size = strlen(message);

    ssize_t n = write(client_sockfd, &size, sizeof(size_t));
    if (n < 0)
        ZF_LOGW_STR("ERROR writing to socket");
    else {
        //Send message to server
        n = write(client_sockfd, message, strlen(message));
        if (n < 0)
            ZF_LOGW_STR("ERROR writing to socket");
    }
}

//Receive message from server
void receive_message(void) {
    while (!closeClient) {
        size_t size = 0;
        ssize_t n = read(client_sockfd, &size, sizeof(size_t));
        if (n < 0) // No data to read;
            continue;

        char buffer[size];

        n = read(client_sockfd, buffer, size);

        if (n < 0)
            ZF_LOGW_STR("ERROR reading data from socket");

        if (strstr(buffer, SERVER_DOWN_MESSAGE)) {
            exit(EXIT_SUCCESS);
        }
    }
}

void close_connection(void) {
    close(client_sockfd);
    close(client_sockfd);
}

void close_client() {
    closeClient = 1;
    close_connection();
}