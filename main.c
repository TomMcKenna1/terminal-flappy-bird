#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/ioctl.h>
#include <signal.h>
#include <termios.h>
#include <time.h> 

struct winsize terminal_size;

void set_win_size(){
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal_size);
}

struct game_object {
    float x;
    float y;
    int s;
};

struct termios linux_get_terminal_settings() {
    struct termios terminal_settings
    tcgetattr(STDIN_FILENO, &terminal_settings);
    return terminal_settings;
}

void linux_set_terminal_settings(struct termios terminal_settings) {
    struct termios new;
	new = terminal_settings;
	// Disable canonical mode and input echo
	new.c_lflag &= (~ICANON & ~ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &new);
}

int check_user_input() {
    int bytesWaiting;
    ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}

void draw_current_game_frame(struct game_object bird, struct game_object *pipes, int NUMBER_OF_PIPES, int pipe_opening) {
    int number_of_rows = terminal_size.ws_row;
    int number_of_columns = terminal_size.ws_col; 

    // Move cursor to top left of screen (so new game frame overwrites current screen)
    printf("\033[H");

    for(int row = 1; row <= number_of_rows; row++){
        for(int column = 1; column <= number_of_columns; column++) {
            //Draw border
            if(row == 1 || row == number_of_rows) {
                putchar('-');
            }
            else if(column == 1 || column == number_of_columns) {
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
                        if(row <= pipes[i].y*(terminal_size.ws_row-1-pipe_opening) || row >= (pipes[i].y*(terminal_size.ws_row-1-pipe_opening))+pipe_opening){
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

    signal(SIGWINCH, set_win_size);
    set_win_size();
    
    bird.x = terminal_size.ws_col/8;
    bird.y = (terminal_size.ws_row/3)*2;
    bird.s = 0;
    for(int i = 0; i < NUMBER_OF_PIPES; i++){
        pipes[i].x = terminal_size.ws_col/3 + i*25;

        pipes[i].y = rand()/(float)RAND_MAX;
    }

    draw_current_game_frame(bird, pipes, NUMBER_OF_PIPES, PIPE_GAP);

    struct termios old = linux_get_terminal_settings();
    linux_set_terminal_settings()

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
                if(bird.y > terminal_size.ws_row-2){
                    bird.y = terminal_size.ws_row-1;
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
        // nanosleep(&ts, &ts);
    }

    // Restore old terminal settings
	tcsetattr(STDIN_FILENO, TCSANOW, &old);
    return 0;
}