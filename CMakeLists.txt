cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 14)

project(ChaTTY)


set(CURSES_NEED_NCURSES TRUE)


find_package(Curses REQUIRED)

include_directories(
    common/
    ${CURSES_INCLUDE_DIR}
    )

file(GLOB ChaTTY_Sources client/*.cpp)
file(GLOB ChaTTY_Headers common/*.h client/*.hpp)
file(GLOB ChaTTY_Sources_server server/*.cpp)
file(GLOB ChaTTY_Headers_server common/*.h server/*.hpp)

add_executable(ChaTTY ${ChaTTY_Sources} ${ChaTTY_Headers})
add_executable(ChaTTYd ${ChaTTY_Sources_server} ${ChaTTY_Headers_server})

target_link_libraries(ChaTTY
    ${CURSES_LIBRARIES}
    )

target_include_directories(ChaTTY PRIVATE client/)
target_include_directories(ChaTTYd PRIVATE server/)
