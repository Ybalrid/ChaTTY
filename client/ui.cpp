#include "ui.hpp"
#include <ncurses.h>

void pop_back_utf8(ustring& utf8)
{
    if(utf8.empty())
        return;

    auto cp = utf8.data() + utf8.size();
    while(--cp >= utf8.data() && ((*cp & 0b10000000) && !(*cp & 0b01000000))) {}
    if(cp >= utf8.data())
        utf8.resize(cp - utf8.data());
}

UserInterface* UserInterface::singleton = nullptr;
const unsigned char UserInterface::invite[3] = "> ";

UserInterface::UserInterface() :
    stop{false},
    maxx{0},
    maxy{0},
    func_ptr_to_server{nullptr}
{
    if(singleton)
    {
        fprintf(stderr, "Error, can't init the UI 2 times!");
        exit(-1);
    }

    singleton = this;
    //Init ncurses
    terminal = initscr();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_RED);
    init_pair(2, COLOR_RED, COLOR_WHITE);
    attron(COLOR_PAIR(1));
    curs_set(2);
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

UserInterface& UserInterface::get_singleton()
{
    return *singleton;
}

void UserInterface::hook_send_messages(void (*fpointer)(const char*))
{
    func_ptr_to_server = fpointer;
}

void UserInterface::send_to_server(ustring message)
{
    if(func_ptr_to_server) func_ptr_to_server(reinterpret_cast<const char*>(message.c_str()));
}

std::string UserInterface::ask_for_username()
{
    werase(chatLog);
    box(chatLog, 0, 0);
    wmove(chatLog, 1, 1);
    wattron(chatLog, COLOR_PAIR(2));
    wprintw(chatLog, "\xe2\x9c\x93 Please, enter your name !");
    wattroff(chatLog, COLOR_PAIR(2));

    werase(inputLine);
    box(inputLine, 0, 0);
    wmove(inputLine, 1, 1);
    refresh();
    wrefresh(chatLog);
    wrefresh(inputLine);

    std::string name;
    bool finished = false;
    while(!finished)
    {
        int input = getch();
        if(input == ERR) continue;

        if(input == 127)
        {
            if(!name.empty())
                name.pop_back();
        }
        if(input >= 32 && input <= 126)
        {
            name.push_back(static_cast<char>(input));
        }
        if(input == '\n' || input == '\r')
        {
            if(!name.empty())
                finished = true;
        }

        werase(inputLine);
        box(inputLine, 0, 0);
        wmove(inputLine, 1, 1);
        wprintw(inputLine, name.c_str());
        refresh();
        wrefresh(inputLine);
    }

    werase(chatLog);
    werase(inputLine);
    box(chatLog, 0, 0);
    box(inputLine, 0, 0);
    refresh();
    wrefresh(chatLog);
    wrefresh(inputLine);
    wmove(inputLine, 1, 1);

    return name;
}

void UserInterface::event_loop()
{
    werase(chatLog);
    box(chatLog, 0, 0);
    wmove(chatLog, 1, 1);
    wprintw(chatLog, "Okay, I can write on this thing!");
    werase(inputLine);
    box(inputLine, 0, 0);
    wmove(inputLine, 1, 1);
    wprintw(inputLine, reinterpret_cast<const char*>((invite + inputBuffer).c_str()));

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
                pop_back_utf8(inputBuffer);
        }
        //TODO properly handler characters
        else if(input >= 32 && input <= 126) //ASCII, delete char removed
            inputBuffer.push_back(static_cast<unsigned char>(input));
        else if((input & 0xE0) == 0xC0) //2 bytes encoded UTF-8 codepoint
        {
            inputBuffer.push_back(static_cast<unsigned char>(input));
            inputBuffer.push_back(static_cast<unsigned char>(getch()));
        }
        else if((input & 0xF0) == 0xE0) //3 bytes encoded UTF-8 codepoint
        {
            inputBuffer.push_back(static_cast<unsigned char>(input));
            inputBuffer.push_back(static_cast<unsigned char>(getch()));
            inputBuffer.push_back(static_cast<unsigned char>(getch()));
        }
        else if((input & 0xF8) == 0xF0) //4 bytes encoded UTF-8 codepoint
        {
            inputBuffer.push_back(static_cast<unsigned char>(input));
            inputBuffer.push_back(static_cast<unsigned char>(getch()));
            inputBuffer.push_back(static_cast<unsigned char>(getch()));
            inputBuffer.push_back(static_cast<unsigned char>(getch()));
        }

        if(input == '\n' || input == '\r')
        {
            //send input buffer text to network code
            send_to_server(inputBuffer);
            inputBuffer.clear();
        }
    }
}

bool UserInterface::user_wants_to_quit()
{
    return stop;
}


