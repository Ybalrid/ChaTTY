#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <ncurses.h>

void tryNcurses()
{
    setlocale(LC_ALL, "");
    initscr();
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    curs_set(2);
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
while(1)
{
    int ch = getch();

        attron(A_BOLD);
        printw("%d\n", ch);
        attroff(A_BOLD);

    refresh();
}
    getch();
    endwin();
}

int main(int argc, char* argv[])
{
    tryNcurses();
    return EXIT_SUCCESS;
}
