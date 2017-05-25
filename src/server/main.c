/**
 * @file
 * @author Mark Said Camilleri
 * @version 20170521.
 */

#include "network_server.h"
#include "arena.h"

pthread_mutex_t gameLockWhenPlayerAddedMutex = PTHREAD_MUTEX_INITIALIZER;

void delay(unsigned int secs) {
    unsigned int retTime = time(NULL) + secs;   // Get finishing time.
    while (time(NULL) < retTime);               // Loop until it arrives.
}

int main(void) {
    int start_sending = 0;

    pthread_t connectionThread;
    if (pthread_create(&connectionThread, NULL, init_server_and_accept_connections, &start_sending) != 0) {
        ZF_LOGF_STR("Error when creating new thread");
        exit(EXIT_FAILURE);
    }

    while (!start_sending);

    init_arena();
    play();

    exit(EXIT_SUCCESS);
}