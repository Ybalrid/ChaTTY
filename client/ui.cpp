#include "ui.hpp"
#include <cstring>
#include <ncurses.h>
void UserInterface::yank_chat_log_to_screen()
{
    int y = 0;
    int maxy, maxx;
    getmaxyx(chatLog, maxy, maxx);
    for(int i = std::max(0, (int)(chatLogBuffer.size() - (maxy  - 2)));
            i < chatLogBuffer.size();
            i++)
    {
        wmove(chatLog, ++y, 1);
        wprintw(chatLog, reinterpret_cast<const char*>(chatLogBuffer[i].c_str()));
    }
}

void UserInterface::yank_buddies_to_screen()
{
    int y = 0;
    int maxx, maxy;
    getmaxy(buddyList);

    for(int i = 0 /*std::max(0, (int)(buddies.size() - (maxy - 2)))*/;
            i < buddies.size();
            i++)
    {
        wmove(buddyList, ++y, 1);
        wprintw(buddyList, reinterpret_cast<const char*>(buddies[i].c_str()));
    }
}
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

void UserInterface::display_message(const unsigned char* username, const unsigned char* message)
{
    ustring s_username{username};
    ustring s_message{message};
    ustring to_add = s_username + reinterpret_cast<const unsigned char*>(": ") + s_message;
    chatLogBuffer.push_back(to_add);
}

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
    timeout(100);
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
    buddyList = newwin(maxy - 2, 32, 1, 1);
    box(buddyList, 0, 0);
    wrefresh(buddyList);

    //window 2 will show the chat log
    chatLog = newwin(maxy - 5, maxx - 35, 1, 34);
    box(chatLog, 0, 0);
    wrefresh(chatLog);

    //window 3 will be the text input bar of the user.
    inputLine = newwin(3, maxx - 35, maxy -4, 34);
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

    ustring name;
    bool finished = false;
    while(!finished)
    {
        int input = getch();
        if(input == ERR) continue;

        if(input == KEY_BACKSPACE
                || input == 127)
        {
            if(!name.empty())
                pop_back_utf8(name);
        }
        else if(input >= 32 && input <= 126) //ASCII, delete char removed
            name.push_back(static_cast<unsigned char>(input));
        else if((input & 0xE0) == 0xC0) //2 bytes encoded UTF-8 codepoint
        {
            name.push_back(static_cast<unsigned char>(input));
            name.push_back(static_cast<unsigned char>(getch()));
        }
        else if((input & 0xF0) == 0xE0) //3 bytes encoded UTF-8 codepoint
        {
            name.push_back(static_cast<unsigned char>(input));
            name.push_back(static_cast<unsigned char>(getch()));
            name.push_back(static_cast<unsigned char>(getch()));
        }
        else if((input & 0xF8) == 0xF0) //4 bytes encoded UTF-8 codepoint
        {
            name.push_back(static_cast<unsigned char>(input));
            name.push_back(static_cast<unsigned char>(getch()));
            name.push_back(static_cast<unsigned char>(getch()));
            name.push_back(static_cast<unsigned char>(getch()));
        }

        if(input == '\n' || input == '\r')
        {
            finished = true;
        }


        werase(inputLine);
        box(inputLine, 0, 0);
        wmove(inputLine, 1, 1);
        wprintw(inputLine, reinterpret_cast<const char*>(name.c_str()));
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

    return reinterpret_cast<const char *>(name.c_str());
}

void UserInterface::event_loop()
{
    werase(chatLog);
    box(chatLog, 0, 0);
    wmove(chatLog, 1, 1);
    yank_chat_log_to_screen();

    werase(buddyList);
    box(buddyList, 0, 0);
    wmove(buddyList, 1, 1);
    yank_buddies_to_screen();

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

void UserInterface::update_user_list(const char* lst)
{
    buddies.clear();
    if (!lst || *lst == '\0')
    {
        //nobody
    }
    else do
    {
        //Get the first string from the list
        ustring name = (reinterpret_cast<const unsigned char*>(lst));
        buddies.push_back(name);

        lst += strlen(lst) + 1; //This advance the start of the string after the end of the current one, because sizeof(char) = 1
    } while (*lst != '\0');//End of the list is marked by \0\0 instead of \0
}


