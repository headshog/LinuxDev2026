#define _POSIX_C_SOURCE 200809L
#include <config.h>

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include <rhash.h>

#ifdef USE_READLINE
#include <readline/history.h>
#include <readline/readline.h>
#endif

#define PROMPT "> "
#define DELIMITERS " \t\r\n"
#define MAX_DIGEST 64

static const struct {
    const char *name;
    unsigned id;
} algorithms[] = {
    {"crc32", RHASH_CRC32},
    {"md4", RHASH_MD4},
    {"md5", RHASH_MD5},
    {"sha1", RHASH_SHA1},
    {"sha224", RHASH_SHA224},
    {"sha256", RHASH_SHA256},
    {"sha384", RHASH_SHA384},
    {"sha512", RHASH_SHA512},
    {"tiger", RHASH_TIGER},
    {"tth", RHASH_TTH},
    {"ed2k", RHASH_ED2K},
    {"aich", RHASH_AICH},
    {"whirlpool", RHASH_WHIRLPOOL},
    {"ripemd160", RHASH_RIPEMD160},
};

static unsigned find_algorithm(const char *name)
{
    size_t i;

    for (i = 0; i < sizeof(algorithms) / sizeof(algorithms[0]); i++)
        if (strcasecmp(name, algorithms[i].name) == 0)
            return algorithms[i].id;
    return 0;
}

/* Handle one command: "ALGORITHM FILE" or "ALGORITHM "STRING". */
static void process(char *line)
{
    unsigned char digest[MAX_DIGEST];
    char output[2 * MAX_DIGEST + 1];
    char *name, *argument;
    size_t length;
    unsigned id;
    int flags, result;

    name = strtok(line, DELIMITERS);
    if (name == NULL)
        return;
    argument = strtok(NULL, DELIMITERS);

    id = find_algorithm(name);
    if (id == 0) {
        fprintf(stderr, "rhasher: unknown hash algorithm '%s'\n", name);
        return;
    }
    if (argument == NULL) {
        fprintf(stderr, "rhasher: %s: file name or \"string expected\n", name);
        return;
    }

    if (argument[0] == '"') {
        /* The closing quote is optional: "abc and "abc" are the same. */
        argument++;
        length = strlen(argument);
        if (length > 0 && argument[length - 1] == '"')
            argument[--length] = '\0';
        result = rhash_msg(id, argument, length, digest);
    } else {
        errno = 0;
        result = rhash_file(id, argument, digest);
    }
    if (result < 0) {
        fprintf(stderr, "rhasher: %s: %s\n", argument,
                errno ? strerror(errno) : "cannot compute hash");
        return;
    }

    flags = isupper((unsigned char)name[0]) ? RHPR_HEX : RHPR_BASE64;
    length = rhash_print_bytes(output, digest, rhash_get_digest_size(id),
                               flags);
    printf("%.*s\n", (int)length, output);
}

int main(void)
{
    char *line = NULL;
    size_t size = 0;
    int interactive = isatty(STDIN_FILENO);

    rhash_library_init();

#ifdef USE_READLINE
    /* Without a terminal readline only adds echo and escapes to stdout. */
    if (interactive) {
        while ((line = readline(PROMPT)) != NULL) {
            if (*line != '\0')
                add_history(line);
            process(line);
            free(line);
        }
        putchar('\n');
        return 0;
    }
#endif

    if (interactive) {
        fputs(PROMPT, stdout);
        fflush(stdout);
    }
    while (getline(&line, &size, stdin) != -1) {
        process(line);
        if (interactive) {
            fputs(PROMPT, stdout);
            fflush(stdout);
        }
    }
    if (interactive)
        putchar('\n');
    free(line);
    return 0;
}
