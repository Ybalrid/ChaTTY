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
    maxy = getmaxy(terminal);
    maxx = getmaxx(terminal);
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

    move(maxy-1, 0);
}

UserInterface::~UserInterface()
{
    endwin();
}


void UserInterface::send_to_server(std::string message)
{
}

void UserInterface::event_loop()
{

    //TODO handle change of size here
    //Update display
    refresh();
    wrefresh(buddyList);
    wrefresh(chatLog);
    wrefresh(inputLine);
    //Capture keyboard without blocking
    int input = getch();
    if(input != ERR)
    {
        //Get the F12 keu
        if(input == KEY_F(12))
        {
            stop = true;
            return;
        }

        //Backspace or delete character
        if(input == KEY_BACKSPACE
                || input == 127)
        {
            if(!inputBuffer.empty())
                inputBuffer.pop_back(); //popping from empty string causes segfault
        }
        //TODO properly handler characters
        if(input >= 32 && input <= 126)
            inputBuffer.push_back(static_cast<char>(input));

        if(input == '\n' || input == '\r')
        {
            //send input buffer text to network code
            send_to_server(inputBuffer);
            inputBuffer.clear();
        }
    }
    werase(inputLine);
    box(inputLine, 0, 0);
    wmove(inputLine, 1, 1);
    wprintw(inputLine, inputBuffer.c_str());
}

bool UserInterface::user_wants_to_quit()
{
    return stop;
}


