#pragma once

#include <string>
#include <ncurses.h>

class UserInterface
{
    std::string inputBuffer;
    bool stop;

    WINDOW* terminal;
    WINDOW* buddyList;
    WINDOW* chatLog;
    WINDOW* InputLine;

    public:
    ///Initialize ncurses. Create 3 windows where to print text
    UserInterface();
    ~UserInterface();

    ///Get event (text typed) from the terminal, in a non blocking way and do something with it
    ///Also capture non-text commands and change the state of flags accordigly
    void event_loop();

    ///Return true if the user requested to quit the program
    bool user_wants_to_quit();
};
