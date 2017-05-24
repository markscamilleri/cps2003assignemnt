/**
 * @file
 * @author Mark Said Camilleri
 * @version 20170521
 */

#ifndef SERVER_ARENA_H
#define SERVER_ARENA_H

/**
 * Map tile definitions
 */
/// Fruit in tile
#define FRUIT -1
/// Empty tile
#define EMPTY 0
/// Snake in tile
#define SNAKE 1

#include <stddef.h>
#include <malloc.h>
#include "snake.h"
#include "../config.h"

/**
 * Initializes the arena and game
 */
void init_arena(void);

/**
 * Creates a new player and snake and adds it to the array of players
 * @param playerNum the player's number/sockfd
 */
void createPlayer(int playerNum);

/**
 * Moves this snake by one unit to the direction it's facing.
 * @param snake
 */
void moveSnake(Snake *snake);

/**
 * Changes the snake's direction. This does so with mutexes
 * @param snake the snake to change direction
 * @param dir the new direction
 */
void changeDirection(Snake *snake, Direction dir);

/**
 * Spawns a fruit in a random location
 * @return the location of this fruit
 */
void spawnFruit();

/**
 * Called when a player has lost
 * @param pSnake the player that lost
 */
void playerLost(Snake *pSnake);

/**
 * Called when a player won
 * @param pSnake the player that won
 */
void playerWon(Snake *pSnake);

/**
 * Called to restart the server
 */
void restart_server();

/**
 * Adds players already connected to the game
 */
void add_players();

/**
 * Starts the game
 */
void play();


#endif  // SERVER_ARENA_H
