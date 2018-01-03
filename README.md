# ChaTTY
A chat for chatty people that runs on your terminal over a LAN

This program uses ncurses, and uses the Linux Kernel system call interface for low level operations. It is **not** compatible with any other OS.

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

ChaTTYd take as first argument the port it will listen to incomming TCP connections

## Client configuration

ChaTTY take as first argument the hostname/ip address of the server it will attempt to connect to.


