#include <linux/ioctl.h>
#include <termios.h>

struct terminal {
    int rows;
    int cols;
};

void set_terminal_settings() {
    struct termios terminal_settings
    tcgetattr(STDIN_FILENO, &terminal_settings);
	// Disable canonical mode and input echo
	terminal_settings.c_lflag &= (~ICANON & ~ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &terminal_settings);
}

struct terminal get_terminal(){
    struct winsize terminal_size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &terminal_size);
    struct terminal terminal;
    terminal.cols = terminal_size.ws_col
    terminal.rows = terminal_size.ws_row
    return terminal
}

int get_input_buffer() {
    int bytesWaiting;
    ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}