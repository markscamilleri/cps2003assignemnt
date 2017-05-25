/**
 * @file
 * @author Mark Said Camilleri
 * @version 20170524.
 */

#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "arena.h"
#include "network_server.h"

int map[MAP_SIZE][MAP_SIZE];

int gameover = 0;

/// Dynamic array of snakes (use malloc and free)
SnakeDynArray players;

pthread_mutex_t playerMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t gameLockWhenPlayerAddedMutex;
pthread_mutex_t connectionListMutex;

void delay_ms(unsigned int ms) {
    struct timeval time;
    struct timeval start_time;

    gettimeofday(&start_time, NULL);

    do {
        gettimeofday(&time, NULL);
    } while (time.tv_sec < start_time.tv_sec && time.tv_usec < start_time.tv_usec + ms);
}

void init_arena(void) {
    gameover = 0;

    // Set up the map to be empty
    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            map[i][j] = EMPTY;
        }
    };

    players.array = malloc(sizeof(Snake));
    players.used = 0;
    players.size = 1;


    pthread_t fruit_tread;
    if (pthread_create(&fruit_tread, NULL, randomlySpawnFruit, NULL) != 0) {
        ZF_LOGF_STR("Unable to create a thread to spawn the fruit");
        exit(EXIT_FAILURE);
    }

    atexit(onExit);
}

int check_position(int x, int y, int dir){
    switch (dir){
        case UP:
            return map[y][x] == EMPTY && map[y-1][x] == EMPTY && map[y-2][x] == EMPTY;
        case LEFT:
            return map[y][x] == EMPTY && map[y][x+1] == EMPTY && map[y][x+2] == EMPTY;
        case RIGHT:
            return map[y][x] == EMPTY && map[y][x-1] == EMPTY && map[y][x-2] == EMPTY;
        case DOWN:
            return map[y][x] == EMPTY && map[y+1][x] == EMPTY && map[y+2][x] == EMPTY;
    }

    return 0;
}

void createPlayer(int playerNum) {
    Snake snake = {.playerNum = playerNum, .score = 3, .positions = malloc(sizeof(Position) * 3)};

    int dir = rand() % 4;

    if (dir == 0) {
        snake.dir = UP;
    } else if (dir == 1) {
        snake.dir = LEFT;
    } else if (dir == 2) {
        snake.dir = RIGHT;
    } else if (dir == 3) {
        snake.dir = DOWN;
    }

    srand(time(NULL));

    int start_x;
    int start_y;

    do {
        start_x = rand() % MAP_SIZE;
        start_y = rand() % MAP_SIZE;

        snake.positions[0].x = start_x;
        snake.positions[0].y = start_y;
    } while (!check_position(start_x, start_y, dir));

        switch (snake.dir) {
            case UP:
                snake.positions[1].x = start_x;
                snake.positions[1].y = start_y - 1;
                snake.positions[2].x = start_x;
                snake.positions[2].y = start_y - 2;
                map[start_y][start_x] = SNAKE;
                map[start_y - 1][start_x] = SNAKE;
                map[start_y - 2][start_x] = SNAKE;
                break;
            case LEFT:
                snake.positions[1].x = start_x + 1;
                snake.positions[1].y = start_y;
                snake.positions[2].x = start_x + 2;
                snake.positions[2].y = start_y;
                map[start_y][start_x] = SNAKE;
                map[start_y][start_x + 1] = SNAKE;
                map[start_y][start_x + 2] = SNAKE;
                break;
            case RIGHT:
                snake.positions[1].x = start_x - 1;
                snake.positions[1].y = start_y;
                snake.positions[2].x = start_x - 2;
                snake.positions[2].y = start_y;
                map[start_y][start_x] = SNAKE;
                map[start_y][start_x - 1] = SNAKE;
                map[start_y][start_x - 2] = SNAKE;
                break;
            case DOWN:
                snake.positions[1].x = start_x;
                snake.positions[1].y = start_y + 1;
                snake.positions[2].x = start_x;
                snake.positions[2].y = start_y + 2;
                map[start_y][start_x] = SNAKE;
                map[start_y + 1][start_x] = SNAKE;
                map[start_y + 2][start_x] = SNAKE;
                break;
        }

        SnakeDynArray_add(&players, snake);

        ZF_LOGD_STR("Sending snake");
        send_player_snake(&snake);
        ZF_LOGD_STR("Sending Scores");
        update_send_scores();
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

        if (newhead.y < 0 || newhead.y >= MAP_SIZE || newhead.x < 0 || newhead.x >= MAP_SIZE ||
            map[newhead.y][newhead.x] == SNAKE) {
            playerLost(snake);
            return;
        }

        if (map[newhead.y][newhead.x] == FRUIT) {
            if (snake->score == 14) {
                playerWon(snake);
                return;
            }
            snake->positions = (Position *) realloc(snake->positions, (++(snake->score)) * sizeof(Position));
        }

        for (int i = snake->score - 1; i > 0; --i) {
            snake->positions[i] = snake->positions[i - 1];
        }

        snake->positions[0] = newhead;

        for (int i = 0; i < snake->score; ++i) {
            map[snake->positions[i].y][snake->positions[i].x] = SNAKE;
        }
    }

    void randomlySpawnFruit() {

        while (!gameover) {
            srand(time(NULL));
            delay_ms((unsigned int) (rand() % 20000)); // delays for a random time

            int x_pos, y_pos;

            do {
                x_pos = rand() % MAP_SIZE;
                y_pos = rand() % MAP_SIZE;
            } while (map[y_pos][x_pos] != EMPTY);

            map[y_pos][x_pos] = FRUIT;
        }
    }

    void playerLost(Snake *pSnake) {
        send_message_to_sockfd(pSnake->playerNum, PLAYER_LOST_MESSAGE);
        SnakeDynArray_removeFromArray(&players, (unsigned int) (pSnake - players.array)); // To get the index (offset)
        disconnect(pSnake->playerNum);
        if (players.used == 0) {
            gameover = 1;
            restart_server();
        }
    }

    void playerWon(Snake *pSnake) {
        char message[255];

        strcat(message, PLAYER_WON_MESSAGE);
        strcat(message, "%d");

        int size = sprintf(message, message, pSnake->playerNum);

        char toSend[size];

        strcpy(toSend, message);

        broadcast_message(toSend);
        gameover = 1;
        restart_server();
    }

    void restart_server() {
        init_arena();
        add_players();
    }

    void add_players() {
        ListNode *currentNode = get_connection_list();
        while (currentNode != NULL) {
            createPlayer(currentNode->newsockfd);
            currentNode = currentNode->next;
        }
    }

    void changeDirection(int playerNum, int dir) {
        Snake *snake = NULL;

        int i;
        for (i = 0; i < players.used; ++i) {
            if (players.array[i].playerNum == playerNum) {
                snake = players.array + i;
                break;
            }
        }

        if (i == players.used || snake == NULL) {
            ZF_LOGF("Unable to find player with id %d", playerNum);
        } else {
            pthread_mutex_lock(&playerMutex); // This will be called from another thread (handling input)
            if (!((snake->dir == UP && dir == DOWN) || (snake->dir == DOWN && dir == UP) ||
                  (snake->dir == LEFT && dir == RIGHT) || (snake->dir == RIGHT && dir == LEFT)))
                snake->dir = dir;
            pthread_mutex_unlock(&playerMutex);
        }
    }

    void play() {
        while (!gameover) {
            pthread_mutex_lock(&gameLockWhenPlayerAddedMutex); //VERY IMPORTANT TO LOCK THIS BEFORE OTHERS!!!
            broadcast_data(map, MESSAGE_TYPE_MAP);
            update_send_scores();
            send_player_snakes(players);
            delay_ms(500); // wait half a second
            for (int i = 0; i < players.used; ++i) {
                moveSnake(players.array + i);
            }
            pthread_mutex_unlock(&gameLockWhenPlayerAddedMutex);
        }
    }

    void update_send_scores() {
        Score scores[players.used];

        for (int i = 0; i < players.used; ++i) {
            scores[i].playerNum = players.array[i].playerNum;
            scores[i].score = players.array[i].score;
        }

        pthread_mutex_lock(&connectionListMutex);
        broadcast_data(scores, MESSAGE_TYPE_SCORES);
        pthread_mutex_unlock(&connectionListMutex);

    }

    void removeSnake(int playerNum) {
        unsigned int i;
        for (i = 0; i < players.used; ++i) {
            if (players.array[i].playerNum == playerNum) {
                pthread_mutex_lock(&playerMutex);
                SnakeDynArray_removeFromArray(&players, i);
                pthread_mutex_unlock(&playerMutex);
                break;
            }
        }
    }

    void onExit() {
        SnakeDynArray_clearArray(&players);
    }