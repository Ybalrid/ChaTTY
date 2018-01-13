#pragma once
#include <stdbool.h>


///Initialize ncurses. Create 3 windows where to print text
void init_ui();

///Get event (text typed) from the terminal, in a non blocking way and do something with it
///Also capture non-text commands and change the state of flags accordigly
void event_loop();

///Return true if the user requested to quit the program
bool user_wants_to_quit();
