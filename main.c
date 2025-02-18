#include <ctype.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h> 

/*** defines ***/
#define CTRL_KEY(k) ((k) & 0x1f)

struct termios org_termios;

void die(const char *s) {
    write(STDOUT_FILENO, "\x1b[2J]", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    perror(s);
    exit(1);
}

void disableRawMode() {
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &org_termios) == -1){
        die("tcsetattr");
    }
}

void enableRawMode() {
    if(tcgetattr(STDIN_FILENO, &org_termios) == -1) die("tcgetattr") ;
    atexit(disableRawMode);
    struct termios raw = org_termios;

    tcgetattr(STDIN_FILENO, &raw);
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag &= ~(CS8);
    raw.c_lflag &= ~(ECHO | IEXTEN | ICANON | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

// Terminal

char editorReadKey (){
    int nread;
    char c;

    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if(nread == -1 && errno != EAGAIN) die("read");
    }
    return c;
}

void editorProcessKey(){
    char c = editorReadKey();
    switch (c){
        case CTRL_KEY('q'):
        exit(0);
        break;
    }
}

void editorRefreshScreen(){
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}

int main() {
    enableRawMode();
    while (1) {
        editorRefreshScreen();
        editorProcessKey();
        write(STDOUT_FILENO, "\x1b[2J]", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);
        // char c = '\0';
        // if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) die("read");
        // if (iscntrl(c)) {
        //     printf("%d\r\n", c);
        // }
        // else {
        //     printf("%d ('%c)\r\n", c, c);
        // }
        // if (c == CTRL_KEY('q'))
        //     break;
    }
    return 0;
}