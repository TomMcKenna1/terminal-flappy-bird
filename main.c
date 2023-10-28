#include "linuxterminal.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h> 

struct game_object {
    float x;
    float y;
    int s;
};

struct terminal terminal;
struct terminal terminal;

void draw_current_game_frame(struct game_object bird, struct game_object *pipes, int NUMBER_OF_PIPES, int pipe_opening) {
    terminal = get_terminal_size();

    // Move cursor to top left of screen (so new game frame overwrites current screen)
    printf("\033[H");

    for(int row = 1; row <= terminal.rows; row++){
        for(int column = 1; column <= terminal.cols; column++) {
            //Draw border
            if(row == 1 || row == terminal.rows) {
                putchar('-');
            }
            else if(column == 1 || column == terminal.cols) {
                putchar('|');
            }
            //Draw bird
            else if(column >= bird.x && column <= bird.x+3 && bird.y == row){
                if (bird.x == column) {
                    putchar('\\');
                    putchar('(');
                    putchar('\'');
                    putchar('>');
                }
            }
            else {
                // Draw pipes
                int checker = 1;
                for(int i = 0; i < NUMBER_OF_PIPES; i++){
                    if(pipes[i].x == column) {
                        if(row <= pipes[i].y*(terminal.rows-1-pipe_opening) || row >= (pipes[i].y*(terminal.rows-1-pipe_opening))+pipe_opening){
                            putchar('#');
                            checker = 0;
                            break;
                        }  
                    }
                }
                if(checker){
                    putchar(' ');
                }
            }
        }
    }

    fflush(stdout);
}

int main(int argc, char **argv){
    int FRAME_DELAY = 50;
    int BIRD_FALL_SPEED = 2;
    int BIRD_SPEED = 2;
    int NUMBER_OF_PIPES = 100;
    int PIPE_GAP = 6;

    int state = 0;
    struct game_object bird;
    struct game_object pipes[NUMBER_OF_PIPES];

    // Set random seed to current time to ensure new pipe y values each execution
    srand(time(NULL));
    terminal = get_terminal_size();
    
    bird.x = terminal.cols/8;
    bird.y = (terminal.rows/3)*2;
    bird.s = 0;
    for(int i = 0; i < NUMBER_OF_PIPES; i++){
        pipes[i].x = terminal.cols/3 + i*25;

        pipes[i].y = rand()/(float)RAND_MAX;
    }

    draw_current_game_frame(bird, pipes, NUMBER_OF_PIPES, PIPE_GAP);

    struct termios old = get_terminal_settings();
    set_terminal_settings(old);

    int birdClock = 0;
    int pipeClock = 0;

    // Main loop
    while(1){

        // Check keypresses
        int bytesWaiting = check_user_input(); 
        if(bytesWaiting > 0){
        
            // Clear stdin queue and take last input
            int c;
            for(int i = 0; i < bytesWaiting; i++){
                c = getchar();
            }
            // Close game if q is pressed
            if(c == 'q'){
                break;
            }
            // Make bird jump if spacebar pressed
            else if (c == ' '){
                state = 1;
                bird.y -= 4;

                // Stop bird at top edge of game window
                if(bird.y < 2){
                    bird.y = 2;
                }
                birdClock = 0;
            }
        }

        // Update game frame
        draw_current_game_frame(bird, pipes, NUMBER_OF_PIPES, PIPE_GAP);

        if(state == 1){
            // Move bird down according to bird fall speed
            if(birdClock == BIRD_FALL_SPEED){
                // Stop bird at bottom edge of game window
                if(bird.y > terminal.rows-2){
                    bird.y = terminal.rows-1;
                }
                else{
                    bird.y++;
                }
                // Reset clock
                birdClock = 0;
            }
            if(pipeClock == BIRD_SPEED){
                for (int i = 0; i < NUMBER_OF_PIPES; i++){
                    pipes[i].x--;
                }
                // Reset clock
                pipeClock = 0;
            }
            // Refresh game
            birdClock++;
            pipeClock++;
        }
        struct timespec ts;
        ts.tv_sec = FRAME_DELAY / 1000;
        ts.tv_nsec = (FRAME_DELAY % 1000) * 1000000;
        nanosleep(&ts, &ts);
    }

    // Restore old terminal settings
	tcsetattr(STDIN_FILENO, TCSANOW, &old);
    return 0;
}