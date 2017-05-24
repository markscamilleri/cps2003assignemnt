/**
 * @file
 * This defines common configuration between client and server
 * Things like port number, address etc...
 * @author Mark Said Camilleri
 * @version 20170521.
 */

#ifndef ASSIGNMENT_CONFIG_H
#define ASSIGNMENT_CONFIG_H

#define PORT_NUMBER 24689
#define ZF_LOG_LEVEL ZF_LOG_DEBUG
#define SERVER_DOWN_MESSAGE "Server going down"
#define CLIENT_DOWN_MESSAGE "Client Disconnected"
#define PLAYER_LOST_MESSAGE "You Lost"
#define PLAYER_WON_MESSAGE(playerNum) "Player " playerNum "won"

#define MAP_SIZE 1000

#endif //ASSIGNMENT_CONFIG_H
