/**
 * @file
 * This defines common configuration between client and server
 * Things like port number, address etc...
 * @author Mark Said Camilleri
 * @version 20170521.
 */

#ifndef ASSIGNMENT_CONFIG_H
#define ASSIGNMENT_CONFIG_H

#define PORT_NUMBER 9993
#define ZF_LOG_LEVEL ZF_LOG_FATAL
#define SERVER_DOWN_MESSAGE "Server going down"
#define CLIENT_DOWN_MESSAGE "Client Disconnected"
#define PLAYER_LOST_MESSAGE "You Lost"
#define PLAYER_WON_MESSAGE "A winner has been chosen"

#define UP 1
#define LEFT 2
#define RIGHT 3
#define DOWN 4

#define MESSAGE_TYPE_STRING 0
#define MESSAGE_TYPE_MAP 1
#define MESSAGE_TYPE_PLAYER 2
#define MESSAGE_TYPE_SCORES 3

#define MAP_SIZE 1000

#endif //ASSIGNMENT_CONFIG_H
