
#include "signal.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "termios.h"
#include "unistd.h"

static struct termios previous_config, new_config;

void reset_terminal() {
  printf("\e[m");  // reset color changes
  fflush(stdout);
  printf("\e[?25h");  // show cursor 
  tcsetattr(STDIN_FILENO, TCSANOW, &previous_config);
}

void config_terminal() {
  tcgetattr(STDIN_FILENO, &previous_config);  // get previous config of the terminal
  new_config = previous_config;  // coping config to reset after finishing game

  new_config.c_lflag &= ~(ICANON | ECHO);  // trun off echo and canonical mode
  new_config.c_cc[VMIN] = 0;
  new_config.c_cc[VTIME] = 0;

  tcsetattr(STDIN_FILENO, TCSANOW, &new_config);

  printf("\e[?25l");  // hide cursor
  atexit(reset_terminal);
}

int read_key(char* buffer, int k) {
  if (buffer[k] == '\033' && buffer[k + 1] == '[') {
    switch (buffer[k + 2]) {
      case 'A': return 1;  // up
      case 'B': return 2;  // down
      case 'C': return 3;  // right
      case 'D': return 4;  // left
    }
  }

  return 0;
}

int read_input() {
  char buffer[4096];  // max size of input buffer for the terminal
  int n = read(STDIN_FILENO, buffer, sizeof(buffer));
  int final_key = 0;
  
  // some keys will be missed, but it can be corrected on the next frame
  // arrows has 2 bytes + \0
  for (int k = 0; k <= n - 3; k += 3) {
    int key = read_key(buffer, k);
    if (key == 0) continue;
    final_key = key;
  }

  return final_key;
}

void print_key(int key) {
 if (key == 1) printf("UP\n"); 
 if (key == 2) printf("DOWN\n"); 
 if (key == 3) printf("RIGHT\n"); 
 if (key == 4) printf("LEFT\n"); 
}

int main() {
  config_terminal();

  struct timespec req = {};
  struct timespec rem = {};

  // game loop
  while (1) {
    int key = read_input();
    print_key(key);
    
    req.tv_nsec = 0.1 * 1000000000;  // convert 1m nanosec in 0.1 sec
    nanosleep(&req, &rem);  // manage the interupt signal better?
  }
  
  return 0;
}
