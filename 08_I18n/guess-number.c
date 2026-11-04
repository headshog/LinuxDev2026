#include "config.h"

#include <errno.h>
#include <libintl.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _(message) gettext(message)

int main(void)
{
    int low = 1, high = 100;
    char *answer = NULL;
    size_t capacity = 0;

    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    bind_textdomain_codeset(PACKAGE, "UTF-8");
    textdomain(PACKAGE);

    /* TRANSLATORS: These are the exact answers accepted from the user. */
    const char *yes = _("Yes");
    const char *no = _("No");

    puts(_("Think of an integer between 1 and 100."));
    while (low < high) {
        int middle = low + (high - low) / 2;

        printf(_("Is your number greater than %d? (%s/%s): "), middle, yes, no);
        fflush(stdout);

        ssize_t length = getline(&answer, &capacity, stdin);
        if (length < 0) {
            if (feof(stdin))
                fputs(_("Error: standard input was closed.\n"), stderr);
            else
                fprintf(stderr, _("Error reading input: %s\n"), strerror(errno));
            free(answer);
            return EXIT_FAILURE;
        }
        if (length > 0 && answer[length - 1] == '\n')
            answer[--length] = '\0';

        /* Compare the whole line, including any embedded NUL bytes. */
        if ((size_t)length == strlen(yes) && strcmp(answer, yes) == 0)
            low = middle + 1;
        else if ((size_t)length == strlen(no) && strcmp(answer, no) == 0)
            high = middle;
        else
            fprintf(stderr, _("Please answer exactly \"%s\" or \"%s\".\n"), yes, no);
    }

    printf(_("Your number is %d.\n"), low);
    free(answer);
    return EXIT_SUCCESS;
}
