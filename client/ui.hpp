#pragma once
#include <list>
#include <vector>
#include <string>
#include <ncurses.h>

using ustring = std::basic_string<unsigned char>;
class UserInterface
{
    static const unsigned char invite[3];
    static UserInterface* singleton;

    ustring inputBuffer;
    bool stop;

    WINDOW* terminal;
    WINDOW* buddyList;
    WINDOW* chatLog;
    WINDOW* inputLine;

    int maxx, maxy;

    std::vector<ustring> chatLogBuffer;
    std::vector<ustring> buddies;

    void send_to_server(ustring message);

    void (*func_ptr_to_server)(const char*);
    void yank_chat_log_to_screen();
    void yank_buddies_to_screen();
    public:
    ///Initialize ncurses. Create 3 windows where to print text
    UserInterface();
    ~UserInterface();

    static UserInterface& get_singleton();

    void hook_send_messages(void (*fpointer)(const char*));
    std::string ask_for_username();
    void display_message(const unsigned char* username, const unsigned char* message);
    ///Get event (text typed) from the terminal, in a non blocking way and do something with it
    ///Also capture non-text commands and change the state of flags accordigly
    void event_loop();

    ///Return true if the user requested to quit the program
    bool user_wants_to_quit();
    void update_user_list(const char* list);
};
