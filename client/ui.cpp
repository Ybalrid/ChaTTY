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
    //curs_set(2);
    noecho();
    keypad(stdscr, TRUE);
    cbreak();
    const int maxy = getmaxy(terminal);
    const int maxx = getmaxx(terminal);
    //Create 3 window

    //window 1 shows a list of connected people
    refresh();
    buddyList = newwin(maxy - 2, 20, 1, 1);
    box(buddyList, 0, 0);
    wrefresh(buddyList);

    //window 2 will show the chat log
    chatLog = newwin(maxy - 5, maxx - 23, 1, 22);
    box(chatLog, 0, 0);
    wrefresh(chatLog);

    //window 3 will be the text input bar of the user.
    inputLine = newwin(3, maxx - 23, maxy -4, 22);
    box(inputLine, 0, 0);
    wrefresh(inputLine);
    wmove(inputLine, 1, 1);
    wprintw(inputLine, "You write here!");

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
    refresh();
    wrefresh(buddyList);
    wrefresh(chatLog);
    wrefresh(inputLine);
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


