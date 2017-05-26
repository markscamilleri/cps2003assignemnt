#include <ncurses.h>
#include <pthread.h>
#include "network_client.h"
#include "client_ui.h"

/**
 * @file
 * @author Mark Said Camilleri
 * @version 20170523.
 */

int ch = ' ';

WINDOW *mapWindow, *instructions, *score;

pthread_mutex_t windowMutex = PTHREAD_MUTEX_INITIALIZER;

int max_y, max_x;

int map[MAP_SIZE][MAP_SIZE];

pthread_mutex_t mapMutex = PTHREAD_MUTEX_INITIALIZER;

Snake player;
pthread_mutex_t snakeMutex = PTHREAD_MUTEX_INITIALIZER;

Score *scores;
pthread_mutex_t scoreMutex = PTHREAD_MUTEX_INITIALIZER;

void endwin_wrapper() {
    free(scores);
    pthread_mutex_destroy(&mapMutex);
    pthread_mutex_destroy(&scoreMutex);
    pthread_mutex_destroy(&snakeMutex);
    delwin(mapWindow);
    delwin(score);
    delwin(instructions);
    endwin();
}

void delay(unsigned int secs) {
    unsigned int retTime = time(0) + secs;   // Get finishing time.
    while (time(0) < retTime);               // Loop until it arrives.
}

void init_gui(void) {
    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            map[i][j] = EMPTY;
        }
    }

    while (player.positions == NULL || player.score < 3);    // wait until server sends data


    pthread_t inputThread;

    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);

    if (pthread_create(&inputThread, NULL, handle_input, NULL) != 0) {
        endwin();
        ZF_LOGF_STR("Error when creating new thread to accept input");
        exit(EXIT_FAILURE);
    }

    atexit(endwin_wrapper);

    handle_output();
}


void handle_output() {
    while (!closeClient) {
	clear();
        getmaxyx(stdscr, max_y, max_x);
        handle_map_output();
        handle_score_output();
        handle_instructions_output();
    }
}

void handle_map_output(void) {
    int ishash = 0;
    for (int k = 0; k < MAP_SIZE; ++k) {
        for (int i = 0; i < MAP_SIZE; ++i) {
            if(map[k][i] == '#') {
                ishash = 1;
                break;
            }
        }
    }

    mapWindow = newwin(max_y - 7, max_x - 16, 0, 0);

    int start_col = player.positions[0].x - ((max_x - 16) / 2);
    int start_row = player.positions[0].y + ((max_y - 7) / 2);

    if (start_col < 0)
        start_col = 0;
    else if (start_col > max_x - 16)
        start_col = MAP_SIZE - max_x + 16;
    if (start_row < 0)
        start_row = 0;
    else if (start_row > max_y - 7)
        start_row = MAP_SIZE - max_y + 7;

    pthread_mutex_lock(&mapMutex);
    wclear(mapWindow);
    box(mapWindow, '|', '-');

    for (int i = 1; i < max_y - 8; ++i) {
        for (int j = 1; j < max_x - 17; ++j) {

            chtype c = ' ';

            if (map[start_row - (i - 1)][start_col - 1 + j] == -1) {
                c = 'o';
            } else if (map[start_row - (i - 1)][start_col - 1 + j] == 0) {
                c = ' ';
            } else if (map[start_row - (i - 1)][start_col - 1 + j] == 1) {
                c = '#';
            }

            mvwaddch(mapWindow, i, j, c);
        }
    }
    pthread_mutex_unlock(&mapMutex);

    wrefresh(mapWindow);
}

void handle_instructions_output(void) {
    instructions = newwin(max_y - 7, 16, 0, max_x - 16);

    wclear(instructions);

    box(instructions, '|', '-');

    attron(A_STANDOUT);
    mvwprintw(instructions, 0, 2, "INSTRUCTIONS");
    attroff(A_STANDOUT);
    mvwprintw(instructions, 1, 1, "Hello, welcome");
    mvwprintw(instructions, 2, 1, "to snakes!");
    attron(A_UNDERLINE);
    mvwprintw(instructions, 4, 1, "Controls:");
    attroff(A_UNDERLINE);
    mvwprintw(instructions, 5, 1, "w - up");
    mvwprintw(instructions, 6, 1, "a - left");
    mvwprintw(instructions, 7, 1, "d - right");
    mvwprintw(instructions, 8, 1, "x - down");
    mvwprintw(instructions, 9, 1, "0 - quit");

    mvwprintw(instructions, 12, 1, "You are Player");

    char str[15];
    sprintf(str, "%d", player.playerNum);

    mvwprintw(instructions, 13, (14 - strlen(str)) / 2, str);
    wrefresh(instructions);
}

void handle_score_output(void) {
    score = newwin(7, max_x, max_y - 7, 0);

    wclear(score);

    box(score, '|', '-');

    attron(A_STANDOUT);
    mvwprintw(score, 0, (max_x / 2) - 3, "SCORES");
    attroff(A_STANDOUT);

    pthread_mutex_lock(&scoreMutex);
    if (scores != NULL) {
        for (int i = 0; i < sizeof(scores) / sizeof(Score) && (18 * (i / 5) < max_x - 18); ++i) {
            mvwprintw(score, (i % 5) + 1, (18 * (i / 5)) + 1, "Player %d: \t %d", scores[i].playerNum,
                      scores[i].score);
        }
        pthread_mutex_unlock(&scoreMutex);
    }
    wrefresh(score);
}

void handle_input() {
    while (!closeClient) {
        ch = wgetch(instructions);

        switch (ch) {
            case '0':
                send_message_to_server(CLIENT_DOWN_MESSAGE);
                exit(EXIT_SUCCESS);
            case KEY_RESIZE:
                clear();
                doupdate();
            default:
                send_message_to_server(&ch);
        }
    }
}

void update_map(int pMap[MAP_SIZE][MAP_SIZE]) {
    pthread_mutex_lock(&mapMutex);
    for (int i = 0; i < MAP_SIZE; ++i) {
        for (int j = 0; j < MAP_SIZE; ++j) {
            map[i][j] = pMap[i][j];
        }
    }
    pthread_mutex_unlock(&mapMutex);
}

void update_snake(Snake pSnake) {
    if (!closeClient) {
        pthread_mutex_lock(&snakeMutex);
        player.playerNum = pSnake.playerNum;
        player.score = pSnake.score;
        player.dir = pSnake.dir;
        player.positions = realloc(player.positions, sizeof(*(pSnake.positions)));
        for (int i = 0; i < sizeof(*(pSnake.positions)) / sizeof(Position); ++i) {
            player.positions[i].x = pSnake.positions[i].x;
            player.positions[i].y = pSnake.positions[i].y;
        }
        free(pSnake.positions);
        pthread_mutex_unlock(&snakeMutex);
    }
}

void update_scores(Score pScores[]) {
    if (!closeClient) {
        pthread_mutex_lock(&scoreMutex);
        scores = realloc(scores, sizeof(*pScores));
        for (int i = 0; i < sizeof(scores) / sizeof(Score); ++i) {
            scores[i].playerNum = pScores[i].playerNum;
            scores[i].score = pScores[i].score;
        }
        pthread_mutex_unlock(&scoreMutex);
    }
}

void showExitMessage(char *message) {
    wclear(map);
    wclear(score);
    wclear(instructions);
    clear();
    getmaxyx(stdscr, max_y, max_x);
    mvprintw(max_y / 2, (max_x - strlen(message)) / 2, message);
    mvprintw(max_y / 2 + 1, (max_x - 21) / 2, "Press any key to exit");
    refresh();
    flushinp();
    getch();
    exit(EXIT_SUCCESS);
}
