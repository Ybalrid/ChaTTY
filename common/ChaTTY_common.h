#ifndef CHATTY_COMMON
#define CHATTY_COMMON

#include <stdio.h>
#include <stdlib.h>

static unsigned long getPortFromStr(const char* str)
{
    return strtoul(str, NULL, 10);
}

#endif
