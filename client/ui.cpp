#include "ui.hpp"
#include <curses.h>

void init_ui()
{
    //Init ncurses

    //Create 3 window

    //window 1 shows a list of connected people
    //window 2 will show the chat log
    //window 3 will be the text input bar of the user.

    //Store state information somewhere
}


void event_loop()
{
    //Update display
    //Capture keyboard without blocking
    //Add typed text to a buffer. If '\r' or '\n' detected, hand that text to the network part of the program
}

bool user_wants_to_quit()
{
    //TODO implement flag storing and retreive
    return false; //dummy return for now
}
