#include <windows.h>
#include <conio.h> // for _kbhit and _getch

struct terminal {
    int rows;
    int cols;
};

void set_terminal_settings() {
    // For Windows, you can't directly replicate the behavior of termios, but you can achieve similar effects.
    // You can use the following code to disable line input and echoing:
    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hInput, &mode);
    mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    SetConsoleMode(hInput, mode);
}

struct terminal get_terminal() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    struct terminal terminal;
    terminal.cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    terminal.rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    return terminal;
}

int get_input_buffer() {
    if (_kbhit()) {
        return _kbhit(); // _kbhit() returns the number of characters in the input buffer
    } else {
        return 0;
    }
}