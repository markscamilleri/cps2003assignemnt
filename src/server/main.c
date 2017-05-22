/**
 * @file
 * @author Mark Said Camilleri
 * @version 20170521.
 */

#include "network_server.h"

void delay(unsigned int secs) {
    unsigned int retTime = time(0) + secs;   // Get finishing time.
    while (time(0) < retTime);               // Loop until it arrives.
}

int main(void) {
    int start_sending = 0;

    pthread_t connectionThread;
    if(pthread_create(&connectionThread, NULL, init_server_and_accept_connections, &start_sending) != 0){
        ZF_LOGF_STR("Error when creating new thread");
        exit(EXIT_FAILURE);
    }

    while(!start_sending);

    while (ListNode_size(get_connection_list()) == 0) {
        ZF_LOGI("List ptr: %p", get_connection_list());
        ZF_LOGI("Size: %d", ListNode_size(get_connection_list()));
        delay(10);
    }
    broadcast_message("Hello World!");
    broadcast_message("How are you today?");

    ListNode *connectionList = get_connection_list();

    char *message;
    sprintf(message, "There are %d active connections", ListNode_size(connectionList));
    broadcast_message(message);
    broadcast_message("Let's wait 10 seconds");
    delay(10);

    sprintf(message, "There are %d active connections", ListNode_size(connectionList));
    broadcast_message(message);
    broadcast_message("Let's wait 10 seconds");
    delay(10);

    sprintf(message, "There are %d active connections", ListNode_size(connectionList));
    broadcast_message(message);
    broadcast_message("Let's wait 10 seconds");
    delay(10);

    sprintf(message, "There are %d active connections", ListNode_size(connectionList));
    broadcast_message(message);
    broadcast_message("Let's wait 10 seconds");
    delay(10);

    close_server();
}