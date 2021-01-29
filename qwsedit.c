/*** includes ***/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>


/*** Defines ***/

#define CTRL_KEY(k) ((k) & 0x1f)


/*** Data ***/

struct termios orig_termios;



/*** Terminal ***/

die(const char *s) {

    write(STDOUT_FILENO, "\x1b[2j", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}

/* function used to set term back to original */
void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    }
    
}

void enableRawMode() {

    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) die("tcgetattr");

    /* tcgetattr(STDIN_FILENO, &orig_termios); */
    atexit(disableRawMode); /* on program close set term back to original */

    struct termios raw = orig_termios; 
    /* set term flags */
    raw.c_lflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_lflag &= ~(OPOST);
    raw.c_lflag &= ~(CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    tcgetattr(STDIN_FILENO, &raw);

    raw.c_lflag &= ~(ECHO);

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}


char editorReadKey() {
    int nread;
    char c;

    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) die("read");
    }

    return c;
}


/*** Output ***/

void editorRefreshScreen() {
    write(STDOUT_FILENO, "\x1b[2j", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
}


/***  Input ***/
void editorProcessKeypress() {
    char c = editorReadKey();

    switch (c) {
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2j", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;
    }
}


/*** Init ***/

int main() {


    enableRawMode();

    char c;
    while (1) {
        editorRefreshScreen();
        editorProcessKeypress();
    }
    return 0;
}