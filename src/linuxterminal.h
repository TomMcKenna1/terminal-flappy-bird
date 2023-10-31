#include <sys/ioctl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

struct terminal {
    int rows;
    int cols;
};

struct terminal get_terminal_size(){
    struct winsize terminal_size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal_size);
    struct terminal terminal;
    terminal.cols = terminal_size.ws_col;
    terminal.rows = terminal_size.ws_row;
    return terminal;
}

struct termios get_terminal_settings() {
    struct termios terminal_settings;
    tcgetattr(STDIN_FILENO, &terminal_settings);
    return terminal_settings;
}

void set_terminal_settings(struct termios terminal_settings) {
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