#ifndef _LINUX_GAME_H
#define _LINUX_GAME_H

#define TICS_SEC 18

#define SCREEN_ROWS 25
#define SCREEN_COLS 80

#define START_MAP_X 10
#define START_MAP_Y 5

#define END_MAP_X (SCREEN_COLS - START_MAP_X)
#define END_MAP_Y (SCREEN_ROWS - START_MAP_Y)

#define INITIAL_PLAYER_X 40
#define INITIAL_PLAYER_Y 15

void update_fps();
void clean_screen();
void print_map();
void init_game();
void set_and_print_player();

#endif
