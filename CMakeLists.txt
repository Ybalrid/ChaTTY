cmake_minimum_required(VERSION 3.8)

project(ChaTTY)


set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)

include_directories(includes/)

file(GLOB ChaTTY_Sources src/*.c)
file(GLOB ChaTTY_Headers includes/*.h)

add_executable(ChaTTY ${ChaTTY_Sources} ${ChaTTY_Headers})
