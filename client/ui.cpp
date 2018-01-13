#include "ui.hpp"
#include <ncurses.h>

UserInterface::UserInterface() :
    stop{false}
{
    //Init ncurses
    terminal = initscr();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    curs_set(2);
    noecho();
    keypad(stdscr, TRUE);
    cbreak();
    const int maxy = getmaxy(terminal);
    const int maxx = getmaxx(terminal);
    //Create 3 window

    //window 1 shows a list of connected people
    buddyList = newwin(maxy - 2, 20, 1, 1);
    box(buddyList, 0, 0);
    wborder(buddyList, '|', '|', '-', '-', '+', '+', '+', '+');
    wrefresh(buddyList);

    //window 2 will show the chat log
    //window 3 will be the text input bar of the user.

    move(maxy-1, 0);
    printw("Press F12 to close");
    move(0, 0);
}

UserInterface::~UserInterface()
{
    endwin();
}


void UserInterface::event_loop()
{
    //Update display
    wrefresh(buddyList);
    //Capture keyboard without blocking
    int input = getch();
    if(input != ERR)
    {
        if(input == KEY_F(12))
        {
            stop = true;
            return;
        }
    }
    //Add typed text to a buffer. If '\r' or '\n' detected, hand that text to the network part of the program
}

bool UserInterface::user_wants_to_quit()
{
    return stop;
}
