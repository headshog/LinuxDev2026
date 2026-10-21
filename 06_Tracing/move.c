#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

enum {
    OK, ARGUMENTS, OPEN_INPUT, STAT_INPUT, TYPE_INPUT,
    OPEN_OUTPUT, STAT_OUTPUT, TYPE_OUTPUT, SAME_FILE, TRUNCATE_OUTPUT,
    READ_INPUT, WRITE_OUTPUT, SYNC_OUTPUT, CLOSE_OUTPUT, CLOSE_INPUT,
    REMOVE_INPUT, CLEAN_CLOSE_OUTPUT, CLEAN_CLOSE_INPUT, CLEAN_REMOVE_OUTPUT
};

static int error(int code, const char *operation, const char *name)
{
    int saved_errno = errno;
    fprintf(stderr, "move: %s '%s': %s\n", operation, name,
            strerror(saved_errno));
    return code;
}

int main(int argc, char *argv[])
{
    int input = -1, output = -1;
    int remove_output = 0, result = OK;
    struct stat input_stat, output_stat;
    char buffer[8192];
    ssize_t count;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s infile outfile\n", argv[0]);
        return ARGUMENTS;
    }

    input = openat(AT_FDCWD, argv[1],
                   O_RDONLY | O_CLOEXEC | O_NOFOLLOW | O_NONBLOCK);
    if (input < 0)
        return error(OPEN_INPUT, "open input", argv[1]);
    if (fstat(input, &input_stat) < 0) {
        result = error(STAT_INPUT, "stat input", argv[1]);
        goto cleanup;
    }
    if (!S_ISREG(input_stat.st_mode)) {
        errno = EINVAL;
        result = error(TYPE_INPUT, "input is not a regular file", argv[1]);
        goto cleanup;
    }

    /* Do not truncate an existing file before checking its inode. */
    output = openat(AT_FDCWD, argv[2], O_WRONLY | O_CREAT | O_EXCL |
                    O_CLOEXEC | O_NOFOLLOW | O_NONBLOCK, 0666);
    if (output >= 0)
        remove_output = 1;
    else if (errno == EEXIST)
        output = openat(AT_FDCWD, argv[2],
                        O_WRONLY | O_CLOEXEC | O_NOFOLLOW | O_NONBLOCK);
    if (output < 0) {
        result = error(OPEN_OUTPUT, "open output", argv[2]);
        goto cleanup;
    }
    if (fstat(output, &output_stat) < 0) {
        result = error(STAT_OUTPUT, "stat output", argv[2]);
        goto cleanup;
    }
    if (!S_ISREG(output_stat.st_mode)) {
        errno = EINVAL;
        result = error(TYPE_OUTPUT, "output is not a regular file", argv[2]);
        goto cleanup;
    }
    if (input_stat.st_dev == output_stat.st_dev &&
        input_stat.st_ino == output_stat.st_ino) {
        errno = EINVAL;
        result = error(SAME_FILE, "input and output are the same file", argv[2]);
        goto cleanup;
    }
    remove_output = 1;
    if (ftruncate(output, 0) < 0) {
        result = error(TRUNCATE_OUTPUT, "truncate output", argv[2]);
        goto cleanup;
    }

    for (;;) {
        count = read(input, buffer, sizeof(buffer));
        if (count < 0) {
            if (errno == EINTR)
                continue;
            result = error(READ_INPUT, "read input", argv[1]);
            goto cleanup;
        }
        if (count == 0)
            break;
        for (ssize_t offset = 0; offset < count;) {
            ssize_t written = write(output, buffer + offset,
                                    (size_t)(count - offset));
            if (written < 0 && errno == EINTR)
                continue;
            if (written <= 0) {
                if (written == 0)
                    errno = EIO;
                result = error(WRITE_OUTPUT, "write output", argv[2]);
                goto cleanup;
            }
            offset += written;
        }
    }
    while (fsync(output) < 0) {
        if (errno == EINTR)
            continue;
        result = error(SYNC_OUTPUT, "sync output", argv[2]);
        goto cleanup;
    }

    /* On Linux a failed close must not be retried. */
    int fd = output;
    output = -1;
    if (close(fd) < 0) {
        result = error(CLOSE_OUTPUT, "close output", argv[2]);
        goto cleanup;
    }
    fd = input;
    input = -1;
    if (close(fd) < 0) {
        result = error(CLOSE_INPUT, "close input", argv[1]);
        goto cleanup;
    }
    if (unlinkat(AT_FDCWD, argv[1], 0) < 0) {
        result = error(REMOVE_INPUT, "remove input", argv[1]);
        goto cleanup;
    }
    return OK;

cleanup:
    if (output >= 0 && close(output) < 0)
        result = error(CLEAN_CLOSE_OUTPUT, "cleanup: close output", argv[2]);
    if (input >= 0 && close(input) < 0)
        result = error(CLEAN_CLOSE_INPUT, "cleanup: close input", argv[1]);
    if (remove_output && unlinkat(AT_FDCWD, argv[2], 0) < 0)
        result = error(CLEAN_REMOVE_OUTPUT, "cleanup: remove output", argv[2]);
    return result;
}
