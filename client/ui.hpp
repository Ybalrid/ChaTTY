#pragma once
#include <list>
#include <vector>
#include <string>
#include <ncurses.h>

class UserInterface
{
    static UserInterface* singleton;
    
    using ustring = std::basic_string<unsigned char>;
    ustring inputBuffer;
    bool stop;

    WINDOW* terminal;
    WINDOW* buddyList;
    WINDOW* chatLog;
    WINDOW* inputLine;

    int maxx, maxy;

    std::vector<std::string> chatLogBuffer;

    void send_to_server(ustring message);

    void (*func_ptr_to_server)(const char*);

    public:
    ///Initialize ncurses. Create 3 windows where to print text
    UserInterface();
    ~UserInterface();

    static UserInterface& get_singleton();

    void hook_send_messages(void (*fpointer)(const char*));
    std::string ask_for_username();
    void display_message(const std::string& username, const std::string message);
    void reset_user_list(const std::list<std::string> usernames);

    ///Get event (text typed) from the terminal, in a non blocking way and do something with it
    ///Also capture non-text commands and change the state of flags accordigly
    void event_loop();

    ///Return true if the user requested to quit the program
    bool user_wants_to_quit();
};
