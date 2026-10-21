#define _GNU_SOURCE
#include <dlfcn.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int unlinkat(int dirfd, const char *pathname, int flags)
{
    const char *name = strrchr(pathname, '/');
    int (*original)(int, const char *, int);
    void *symbol;

    name = name ? name + 1 : pathname;
    if (strstr(name, "PROTECT")) {
        errno = EPERM;
        return -1;
    }

    dlerror();
    symbol = dlsym(RTLD_NEXT, "unlinkat");
    if (dlerror() || !symbol) {
        errno = ENOSYS;
        return -1;
    }
    /* POSIX permits using a dlsym result as a function pointer. */
    memcpy(&original, &symbol, sizeof(original));
    return original(dirfd, pathname, flags);
}
