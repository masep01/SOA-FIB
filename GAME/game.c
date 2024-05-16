#include <game.h>
#include <libc.h>

char MAP[SCREEN_ROWS][SCREEN_COLS] = {
"                                                                                ",
"                                                                                ",
"                                                                                ",
"                                                                                ",
"                                                                                ",
"          ____________________________________________________________          ",
"          |                                                          |          ",
"          |   F     ____   ______       ______   ____                |          ",
"          |        |    | |      |  F  |     |  |    |               |          ",
"          |        |    | |      |_____|     |  | F  |     F         |          ",
"          |        |    | |         F        |  |    |               |          ",
"          | F      |    | |   _____   _____  |  |    |               |          ",
"          |        |      |  |     | |     | |       |               |          ",
"          |        |   ___|  |     | |     | |___    |               |          ",
"          |        |   |     |_____| |_____|     |   |         F     |          ",
"          |        |___|                         |                   |          ",
"          |      F |        _____     _____      |____               |          ",
"          |        |       |     |___|     |          |              |          ",
"          |        |                          F                      |          ",
"          ____________________________________________________________          ",
"                                                                                ",
"                                                                                ",
"                                                                                ",
"                                                                                ",
"                                                                                "
};

int player_x, player_y = 0;

int fps = 0;
int frames = 0;

int tics_prev = 0;

void update_fps(){
  fps = frames;
}

void print_fps(){
  gotoxy(5,2);
  write(1, "FPS: ", sizeof("FPS: "));
  char buff[24];
  itoa(fps, buff);
  gotoxy(10,2);
  write(1, &buff, sizeof(1));
}

void clean_screen(){
  for(int x=0; x<SCREEN_COLS;++x){
    for(int y=0; y<SCREEN_ROWS;++y){
      gotoxy(x,y);
      write(1, "", sizeof(""));
    }
  }
}

void print_map(){
  for(int x=START_MAP_X; x<END_MAP_X;++x){
    for(int y=START_MAP_Y; y<END_MAP_Y;++y){
      gotoxy(x,y);
      write(1, &MAP[y][x], sizeof(char));
    }
  }
}

void set_and_print_player(int x, int y){
  player_x = x;
  player_y = y;

  gotoxy(player_x, player_y);
  write(1, "C", sizeof("C"));
}

void init_game(){
  
  clean_screen();
  print_map();

  set_and_print_player(INITIAL_PLAYER_X,INITIAL_PLAYER_Y);
  
  while(1){
    int current_tics = gettime();
    if(current_tics-tics_prev >= TICS_SEC){
      tics_prev = current_tics;
      update_fps();
      print_fps();
      frames = 0;

    } else {
      print_map();
      set_and_print_player(player_x, player_y);
      ++frames;
    }
  }
}