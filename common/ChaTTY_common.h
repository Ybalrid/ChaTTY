#ifndef CHATTY_COMMON
#define CHATTY_COMMON

#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 500

static unsigned long getPortFromStr(const char* str)
{
    return strtoul(str, NULL, 10);
}

#endif
