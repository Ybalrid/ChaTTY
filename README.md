# ChaTTY
A client/server minimalist chat program for chatty people that runs on your terminal over a LAN, or on the internet.

This program uses ncurses, and uses the Linux Kernel system call interface for low level operations. It is **not** compatible with any other OS.

Made as homework for the system's programing class at ESIEA by Arthur Brainville (@Ybalrid) and Malek Haissous (@skytux)

This is a free and open source program licensed under the terms GNU GPL v3 

## Build instruction

This program uses CMake to generate a build system. For using simple UNIX makefiles do the following

```bash
mkdir build
cd build
cmake ..
make
```

This will produce an executable called `ChaTTY` that serves as a client, and another called ChaTTYd, that serve as a server.

## Server configuration

ChaTTYd take as first argument the *port* it will listen on for incomming TCP connections

## Client configuration

ChaTTY take as first argument the *hostname/ip address of the server* it will attempt to connect to.
The second argument is the *port* to use.


