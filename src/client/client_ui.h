/**
 * @file
 * @author Mark Said Camilleri
 * @version 20170523.
 */

#ifndef ASSIGNMENT_CLIENT_UI_H
#define ASSIGNMENT_CLIENT_UI_H

#include "../server/snake.h"
#include "../server/arena.h"

void init_gui(void);

void handle_output();

void handle_input();

void handle_map_output(void);

void handle_instructions_output(void);

void handle_score_output(void);

void update_map(int pMap[MAP_SIZE][MAP_SIZE]);

void update_snake(Snake pSnake);

void update_scores(Score pScores[]);

void showExitMessage(char * message);

#endif //ASSIGNMENT_CLIENT_UI_H
