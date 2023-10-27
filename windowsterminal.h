#include <windows.h>

struct terminal {
    int rows;
    int cols;
};

struct terminal get_terminal_size(){
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    struct terminal terminal;
    terminal.cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    terminal.rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    return terminal;
}

// struct termios get_terminal_settings() {
//     struct termios terminal_settings
//     tcgetattr(STDIN_FILENO, &terminal_settings);
//     return terminal_settings;
// }

// void set_terminal_settings(struct termios terminal_settings) {
//     struct termios new;
// 	new = terminal_settings;
// 	// Disable canonical mode and input echo
// 	new.c_lflag &= (~ICANON & ~ECHO);
// 	tcsetattr(STDIN_FILENO, TCSANOW, &new);
// }

// int check_user_input() {
//     int bytesWaiting;
//     ioctl(STDIN_FILENO, FIONREAD, &bytesWaiting);
//     return bytesWaiting;
// }