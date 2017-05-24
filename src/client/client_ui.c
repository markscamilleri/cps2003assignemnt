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

WINDOW *map, *instructions, *score;

pthread_mutex_t windowMutex = PTHREAD_MUTEX_INITIALIZER;

int max_y, max_x;


void endwin_wrapper() { endwin(); }

void delay(unsigned int secs) {
    unsigned int retTime = time(0) + secs;   // Get finishing time.
    while (time(0) < retTime);               // Loop until it arrives.
}

void init_gui(void) {
    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);

    pthread_t inputThread;
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
    map = newwin(max_y - 7, max_x - 16, 0, 0);

    wclear(map);

    box(map, '|', '-');

    wrefresh(map);
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

    mvwprintw(instructions, 10, 1, "You pressed ");

    waddch(instructions, ch);
    wrefresh(instructions);
}

void handle_score_output(void) {
    score = newwin(7, max_x, max_y - 7, 0);

    wclear(score);

    box(score, '|', '-');

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
                break;
        }
    }
}

void update_output() {
    if (pthread_mutex_trylock(&windowMutex) == 0) {
        doupdate();
        pthread_mutex_unlock(&windowMutex);
    }
}