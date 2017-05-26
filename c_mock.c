#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

/**
 * @file
 * @author Mark Said Camilleri
 * @version 20170523.
 */

WINDOW *mapWindow, *instructions, *score;

int player = 0;

int max_y, max_x;

void endwin_wrapper() {
    delwin(mapWindow);
    delwin(score);
    delwin(instructions);
    endwin();
}

void handle_map_output(void) {
    mapWindow = newwin(max_y - 7, max_x - 16, 0, 0);

    wclear(mapWindow);

    box(mapWindow, '|', '-');

    for (size_t i = 0; i < 10; i++) {
        int randPosX = (rand() % (max_x - 13)) + 1;
        int randPosY = (rand() % (max_y - 5)) + 1;
        mvwaddch(mapWindow, randPosY, randPosX, 'o');
    }

    mvwprintw(mapWindow, (max_y - 7)/2, (max_x-16)/2, "####");
    mvwprintw(mapWindow, (max_y - 7)/2 + 1, (max_x-16)/2 + 3, "#");

    for (size_t i = 0; i < 5; i++) {
        int randPosX = (rand() % (max_x - 13)) + 1;
        int randPosY = (rand() % (max_y - 5)) + 1;

        int dir = rand() % 2;
        if (dir == 0) {
            mvwprintw(mapWindow, randPosY, randPosX, "######");
        } else if (dir == 1) {
            for (size_t i = 0; i < 4; i++) {
                mvwaddch(mapWindow, randPosY + i, randPosX, '#');
            }
        }
    }


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
    sprintf(str, "%d", player);

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

        for (int i = 0; i < 10; ++i) {
            int points = 0;
            if(i == player) points = 5;
            else points = rand() % 15;
            mvwprintw(score, (i % 5)+1, (32 * (i / 5))+1, "Player %d: \t %d", i, points);
        }
    wrefresh(score);
}


int main(void) {
    int randNo = time(NULL);
    srand(randNo);

    for(int i = 0; i < randNo; ++i){
        srand(randNo);
        randNo = rand();
    }

    initscr();
    cbreak();
    noecho();
    curs_set(FALSE);

    player = rand()% 10;

    clear();
    getmaxyx(stdscr, max_y, max_x);
    handle_map_output();
    handle_score_output();
    handle_instructions_output();

    atexit(endwin_wrapper);

    while(1);

    return 0;
}
