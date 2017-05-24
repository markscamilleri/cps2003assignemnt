/**
 * @file
 * @author Mark Said Camilleri
 * @version 20170524.
 */

#ifndef ASSIGNMENT_SNAKE_H
#define ASSIGNMENT_SNAKE_H

#include <stddef.h>

typedef enum Direction {
    UP = 0,
    LEFT,
    RIGHT,
    DOWN
} Direction;

typedef struct Position {
    int x;
    int y;
} Position;

typedef struct Snake {
    int playerNum;        ///< The player that owns and controls this snake
    int score;             ///< Keeps track of how long the snake is and score
    Position * positions; ///< The snake itself and where it is
    Direction dir;        ///< The direction the snake is moving at
} Snake;

typedef struct SnakeDynArray{
    Snake * array;
    size_t used;
    size_t size;
} SnakeDynArray;

void SnakeDynArray_add(SnakeDynArray arr, Snake element);

void SnakeDynArray_removeFromArray(SnakeDynArray arr, unsigned int n);

void SnakeDynArray_clearArray(SnakeDynArray arr);


#endif //ASSIGNMENT_SNAKE_H
