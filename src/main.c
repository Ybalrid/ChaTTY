#include <stdio.h>
#include <stdlib.h>

#include <ncurses.h>

void tryNcurses()
{
    initscr();
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    curs_set(2);
    raw();
    keypad(stdscr, TRUE);
    noecho();

    printw("Type any character to see it in bold\n");
    int ch = getch();

    if(ch == KEY_F(1))
        printw("F1 has been pressed!");
    else
    {
        printw("The pressed key is ");
        attron(A_BOLD);
        printw("%c", ch);
        attroff(A_BOLD);
    }

    refresh();
    getch();
    endwin();
}

int main(int argc, char* argv[])
{
    tryNcurses();
    return EXIT_SUCCESS;
}
