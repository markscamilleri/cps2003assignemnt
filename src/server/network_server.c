/**
 * @file
 * @author Mark Said Camilleri
 * @version 20170521.
 */

#ifndef ASSIGNMENT_NETWORK_SERVER_C
#define ASSIGNMENT_NETWORK_SERVER_C

#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include "network_server.h"
#include "arena.h"

ListNode *connectionList = NULL;
pthread_mutex_t connectionListMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t gameLockWhenPlayerAddedMutex;


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

    atexit(close_server);

    //Marking socket as a socket that will be used to accept incoming connection requests
    ZF_LOGD_STR("Listening for connections");
    listen(server_sockfd, 5);
    clilen = sizeof(cli_addr);

    signal(SIGPIPE, SIG_IGN);
    *start_sending = 1;

    //Accepting an incoming connection request
    while (!closeServer) {
        accept_connection((struct sockaddr *) &cli_addr, &clilen);
    }
}

void accept_connection(struct sockaddr *cli_addr, socklen_t *clilen) {
    int newsockfd = accept(server_sockfd, cli_addr, clilen);
    pthread_mutex_lock(&gameLockWhenPlayerAddedMutex);
    if (newsockfd < 0)
        ZF_LOGW_STR("ERROR on accepting connection request");
    else {
        if (connectionList == NULL) {
            pthread_mutex_lock(&connectionListMutex);
            connectionList = malloc(sizeof(ListNode));
            connectionList->newsockfd = newsockfd;
            connectionList->next = NULL;
            pthread_mutex_unlock(&connectionListMutex);
        } else {
            pthread_mutex_lock(&connectionListMutex);
            ListNode_add(connectionList, newsockfd);
            pthread_mutex_unlock(&connectionListMutex);
        }
        ZF_LOGD("There are %d nodes in the list", ListNode_size(connectionList));

        createPlayer(newsockfd);

        pthread_mutex_unlock(&gameLockWhenPlayerAddedMutex);
        pthread_t connectionThread;
        if (pthread_create(&connectionThread, NULL, receive_message, &newsockfd) != 0) {
            ZF_LOGF_STR("Error when creating new thread to accept incoming messages");
            exit(EXIT_FAILURE);
        }
    }
}

void close_all_connections(ListNode *node) {
    if (node != NULL) {
        close(node->newsockfd);
        close_all_connections(node->next);
    }
}

void close_all(void) {
    broadcast_message(SERVER_DOWN_MESSAGE);
    close(server_sockfd);
    //Lock connection list
    pthread_mutex_lock(&connectionListMutex);

    close_all_connections(connectionList);
    ListNode_clear(connectionList);

    //Unlock and destroy connection list
    pthread_mutex_unlock(&connectionListMutex);
    pthread_mutex_destroy(&connectionListMutex);
}

void send_str_message_to_client(ListNode *node, char *message) {
    if (node != NULL) {
        send_message_to_sockfd(node->newsockfd, message);
    }
}

void send_message_to_sockfd(int sockfd, char *message) {
    if (ListNode_getNodeIndexFromSockfd(connectionList, sockfd, 0) >= 0) {
        size_t size = strlen(message);

        ssize_t n = write(sockfd, &size, sizeof(size_t));
        if (n < 0) {
            if (errno == EPIPE)
                disconnect(sockfd);
            ZF_LOGW_STR("ERROR writing to socket");
        } else {
            int type = MESSAGE_TYPE_STRING;

            n = write(sockfd, &type, sizeof(int));

            if (n < 0) {
                if (errno == EPIPE)
                    disconnect(sockfd);
                ZF_LOGW_STR("ERROR writing to socket");
            } else {

                n = write(sockfd, message, size);
                if (n < 0) {
                    if (errno == EPIPE)
                        disconnect(sockfd);
                    ZF_LOGW_STR("ERROR writing to socket");
                }
            }
        }
    }
}

void send_message_to_list(ListNode *node, char *message) {
    if (node != NULL) {
        send_str_message_to_client(node, message);
        send_message_to_list(node->next, message);
    }
}

void broadcast_message(char *message) {
    send_message_to_list(connectionList, message);
}

void send_data_to_sockfd(int sockfd, void *data, int type) {
    if (type == MESSAGE_TYPE_STRING)
        send_message_to_sockfd(sockfd, data);
    else if (type == MESSAGE_TYPE_MAP)
        send_map_to_sockfd(sockfd, data);
    else {
        if (ListNode_getNodeIndexFromSockfd(connectionList, sockfd, 0) >= 0) {
            size_t size = sizeof(data);

            ssize_t n = write(sockfd, &size, sizeof(size_t));
            if (n < 0) {
                if (errno == EPIPE)
                    disconnect(sockfd);
                ZF_LOGW_STR("ERROR writing to socket");
            } else {
                n = write(sockfd, &type, sizeof(int));

                if (n < 0) {
                    if (errno == EPIPE)
                        disconnect(sockfd);
                    ZF_LOGW_STR("ERROR writing to socket");
                } else {
                    n = write(sockfd, data, size);
                    if (n < 0) {
                        if (errno == EPIPE)
                            disconnect(sockfd);
                        ZF_LOGW_STR("ERROR writing to socket");
                    }
                }
            }
        }
    }
}

void send_map_to_sockfd(int sockfd, int map[MAP_SIZE][MAP_SIZE]) {
    size_t size = sizeof(map);

    ssize_t n = write(sockfd, &size, sizeof(size_t));
    if (n < 0) {
        if (errno == EPIPE)
            disconnect(sockfd);
        ZF_LOGW_STR("ERROR writing to socket");
    } else {
        int type = MESSAGE_TYPE_MAP;

        n = write(sockfd, &type, sizeof(int));

        if (n < 0) {
            if (errno == EPIPE)
                disconnect(sockfd);
            ZF_LOGW_STR("ERROR writing to socket");
        } else {

            int ishash = 0;
            for (int k = 0; k < MAP_SIZE; ++k) {
                for (int i = 0; i < MAP_SIZE; ++i) {
                    if(map[k][i] == '#') {
                        ishash = 1;
                        break;
                    }
                }
            }

            for (int i = 0; i < size; ++i) {
                n = write(sockfd, map[i], sizeof(int) * MAP_SIZE);
                if (n < 0) {
                    if (errno == EPIPE)
                        disconnect(sockfd);
                    ZF_LOGW_STR("ERROR writing to socket");
                }
            }
        }
    }
}

void send_data_to_list(ListNode *node, void *data, int type) {
    if (node != NULL) {
        send_data_to_sockfd(node->newsockfd, data, type);
        send_data_to_list(node->next, data, type);
    }
}

void broadcast_data(void *data, int type) {
    send_data_to_list(connectionList, data, type);
}


void close_server() {
    closeServer = 1;
    close_all();
}

ListNode *get_connection_list() {
    return connectionList;
}

void receive_message(int clientsockfd) {
    while (!closeServer) {
        size_t size;
        ssize_t n = read(clientsockfd, &size, sizeof(size_t));
        if (n < 0 || size <= 0) // This means that no data is present yet.
            continue;


        char buffer[size];

        n = read(clientsockfd, buffer, size);

        if (n < 0)
            ZF_LOGW_STR("ERROR reading data from socket");

        if (strstr(buffer, CLIENT_DOWN_MESSAGE)) {
            disconnect(clientsockfd);
            pthread_exit(NULL);
        } else if (strcmp(buffer, "u") == 0)
            changeDirection(clientsockfd, UP);
        else if (strcmp(buffer, "x") == 0)
            changeDirection(clientsockfd, DOWN);
        else if (strcmp(buffer, "a") == 0)
            changeDirection(clientsockfd, LEFT);
        else if (strcmp(buffer, "d") == 0)
            changeDirection(clientsockfd, RIGHT);
    }
}

// Resource is already accessed exclusively by this thread, hence, no mutexes.
void disconnect(int clientsockfd) {
    int index = ListNode_getNodeIndexFromSockfd(connectionList, clientsockfd, 0);

    if (index >= 0) {
        ZF_LOGD_STR("Removing node");
        ListNode_remove(connectionList, index);

        removeSnake(clientsockfd);
    }
}

void send_player_snakes(SnakeDynArray players) {
    for (int i = 0; i < players.used; ++i) {
        send_player_snake(players.array + i);
    }

}

void send_player_snake(Snake *player) {
    size_t size = sizeof(Snake);

    ssize_t n = write(player->playerNum, &size, sizeof(size_t));
    if (n < 0) {
        if (errno == EPIPE)
            disconnect(player->playerNum);
        ZF_LOGW_STR("ERROR writing to socket");
    } else {
        int type = MESSAGE_TYPE_PLAYER;
        n = write(player->playerNum, &type, sizeof(int));
        if (n < 0) {
            if (errno == EPIPE)
                disconnect(player->playerNum);
            ZF_LOGW_STR("ERROR writing to socket");
        } else {
            n = write(player->playerNum, player, size);
            if (n < 0) {
                if (errno == EPIPE)
                    disconnect(player->playerNum);
                ZF_LOGW_STR("ERROR writing to socket");
            } else {
                size = sizeof(*(player->positions));
                n = write(player->playerNum, &size, sizeof(size_t));
                if (n < 0) {
                    if (errno == EPIPE)
                        disconnect(player->playerNum);
                    ZF_LOGW_STR("ERROR writing to socket");
                } else {
                    n = write(player->playerNum, player->positions, size);
                    if (n < 0) {
                        if (errno == EPIPE)
                            disconnect(player->playerNum);
                        ZF_LOGW_STR("ERROR writing to socket");
                    }
                }
            }
        }
    }
}

#endif  // ASSIGNMENT_NETWORK_SERVER_C