/**
 * @file
 * @author Mark Said Camilleri
 * @version 20170524.
 */

#include <stdlib.h>
#include <time.h>
#include "arena.h"
#include "network_server.h"

int map[MAP_SIZE][MAP_SIZE];

/// Dynamic array of snakes (use malloc and free)
SnakeDynArray players;

pthread_mutex_t playerMutex = PTHREAD_MUTEX_INITIALIZER;

void init_arena(void) {
    // Set up the map to be empty
    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            map[i][j] = EMPTY;
        }
    };

    players.array = malloc(sizeof(Snake));
    players.used = 0;
    players.size = 1;
}

void createPlayer(int playerNum) {
    Snake snake;
    snake.playerNum = playerNum;
    snake.score = 3;

    srand(time(NULL));
    snake.dir = (Direction) (rand() * 3);

    int start_x = rand() * MAP_SIZE;
    int start_y = rand() * MAP_SIZE;

    snake.positions = malloc(sizeof(Position) * 3);

    snake.positions[0].x = start_x;
    snake.positions[0].y = start_y;

    switch (snake.dir) {
        case UP:
            snake.positions[1].x = start_x;
            snake.positions[1].y = start_y - 1;
            snake.positions[2].x = start_x;
            snake.positions[2].y = start_y - 2;
            break;
        case LEFT:
            snake.positions[1].x = start_x + 1;
            snake.positions[1].y = start_y;
            snake.positions[2].x = start_x + 2;
            snake.positions[2].y = start_y;
            break;
        case RIGHT:
            snake.positions[1].x = start_x - 1;
            snake.positions[1].y = start_y;
            snake.positions[2].x = start_x - 2;
            snake.positions[2].y = start_y;
            break;
        case DOWN:
            snake.positions[1].x = start_x;
            snake.positions[1].y = start_y + 1;
            snake.positions[2].x = start_x;
            snake.positions[2].y = start_y + 2;
            break;
    }

    SnakeDynArray_add(players, snake);
}

void moveSnake(Snake *snake) {
    Position newhead = snake->positions[0];

    switch (snake->dir) {
        case UP:
            newhead.y++;
            break;
        case LEFT:
            newhead.x--;
            break;
        case RIGHT:
            newhead.x++;
            break;
        case DOWN:
            newhead.y--;
            break;
    }

    for (int i = 0; i < snake->score; ++i) {
        map[snake->positions[i].y][snake->positions[i].x] = EMPTY;
    }

    if (newhead.y < 0 || newhead.y >= MAP_SIZE || newhead.x < 0 || newhead.x >= MAP_SIZE || map[newhead.y][newhead.x] == SNAKE){
        playerLost(snake);
        return;
    }

    if (map[newhead.y][newhead.x] == FRUIT) {
        if(snake->score == 14) {
            playerWon(snake);
            return;
        }
        snake->positions = (Position *) realloc(snake->positions, (++snake->score) * sizeof(Position));
    }

    for (int i = snake->score - 1; i > 0; ++i) {
        snake->positions[i] = snake->positions[i - 1];
    }

    snake->positions[0] = newhead;

    for (int i = 0; i < snake->score; ++i) {
        map[snake->positions[i].y][snake->positions[i].x] = SNAKE;
    }
}

void spawnFruit() {
    srand(time(NULL));

    int x_pos, y_pos;

    do {
        x_pos = rand();
        y_pos = rand();
    } while (map[y_pos][x_pos] != EMPTY);

    map[y_pos][x_pos] = FRUIT;
}

void playerLost(Snake *pSnake) {
    send_str_message_to_sockfd(pSnake->playerNum, PLAYER_LOST_MESSAGE);
    SnakeDynArray_removeFromArray(players, (unsigned int) (pSnake - players.array)); // To get the index (offset)
    disconnect(pSnake->playerNum);
}

void playerWon(Snake *pSnake) {
    broadcast_message(PLAYER_WON_MESSAGE(pSnake->playerNum));
    restart_server();
}

void restart_server() {
    init_arena();
    add_players();
    play();
}

void add_players() {
    ListNode * currentNode = get_connection_list();
    while(currentNode != NULL){
        createPlayer(currentNode->newsockfd);
        currentNode = currentNode->next;
    }
}

void changeDirection(Snake *snake, Direction dir) {
    pthread_mutex_lock(&playerMutex);
    snake->dir = dir;
    pthread_mutex_unlock(&playerMutex);
}

void play() {

}

