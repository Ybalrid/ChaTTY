cmake_minimum_required(VERSION 3.8)

project(ChaTTY)


set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)

include_directories(
    includes/
    ${CURSES_INCLUDE_DIR}
    )

file(GLOB ChaTTY_Sources src/*.cpp)
file(GLOB ChaTTY_Headers includes/*.h)

add_executable(ChaTTY ${ChaTTY_Sources} ${ChaTTY_Headers})

target_link_libraries(ChaTTY
    ${CURSES_LIBRARIES}
    )
