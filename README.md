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

This will produce an executable called `ChaTTY`.


