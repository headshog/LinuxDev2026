#include <stdio.h>

#include "outlib.h"

void output(const char *str)
{
        printf("%d: %s\n", Count++, str);
}

void usage(const char *prog)
{
        fprintf(stderr,
                "%s v%.2f: Print all arguments\n"
                "\tUsage: %s arg1 [arg2 [...]]\n",
                prog, VERSION, prog);
}
