/** @file guess-number.c
 * @brief Localized binary-search game, help and documentation generator.
 */
#include "config.h"
#include "roman.h"

#include <errno.h>
#include <getopt.h>
#include <libintl.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Translate an English message using the current gettext domain.
 * @param message English message identifier.
 */
#define _(message) gettext(message)

/** Stable command name used in help, version output and the manual. */
#define PROGRAM_NAME "guess-number"

/** Lower bound of the guessing interval. */
#define GUESS_MIN 1

/** getopt_long key for the documentation-only output mode. */
#define HELP_MD_OPTION 256

/** Long command-line options; the corresponding short forms are -r, -h, -V. */
static const struct option long_options[] = {
    {"roman", no_argument, NULL, 'r'},
    {"help", no_argument, NULL, 'h'},
    {"version", no_argument, NULL, 'V'},
    {"help-md", no_argument, NULL, HELP_MD_OPTION},
    {NULL, 0, NULL, 0}
};

/** Print localized help shared by the terminal, help2man and Doxygen. */
static void print_help(void)
{
    printf(_("Usage: %s [OPTION...]\n"), PROGRAM_NAME);
    printf(_("The computer guesses an integer from %d to %d using binary search.\n\n"),
           GUESS_MIN, ROMAN_MAX);
    puts(_("Options:\n"
           "  -r, --roman      Display numbers as Roman numerals (I..MMMCMXCIX).\n"
           "  -h, --help       Show this help and exit.\n"
           "  -V, --version    Show version and exit.\n"
           "      --help-md    Output English help as a Doxygen main page.\n"));
    printf(_("Answer exactly \"%s\" or \"%s\". Other answers repeat the question.\n"),
           _("Yes"), _("No"));
    puts(_("Closing standard input is an error. At most 12 questions are needed."));
}

/** Generate a Doxygen main-page block containing exactly the English help.
 * The locale is changed only in this process, which exits after generation.
 */
static void print_help_md(void)
{
    setlocale(LC_ALL, "C");
    puts("/**\n@mainpage guess-number\n@verbatim");
    print_help();
    puts("@endverbatim\n*/");
}

/** Format a number for game output.
 * @param value Integer in 1..ROMAN_MAX.
 * @param use_roman Nonzero for Roman notation, zero for decimal.
 * @param buffer Destination containing at least ROMAN_BUFFER_SIZE bytes.
 * @return The supplied buffer.
 */
static const char *number_text(int value, int use_roman,
                               char buffer[ROMAN_BUFFER_SIZE])
{
    if (use_roman)
        int_to_roman(value, buffer, ROMAN_BUFFER_SIZE);
    else
        snprintf(buffer, ROMAN_BUFFER_SIZE, "%d", value);
    return buffer;
}

/** Guess a number using localized, exact yes/no responses.
 * @param use_roman Nonzero to display all numbers in Roman notation.
 * @return EXIT_SUCCESS after guessing, EXIT_FAILURE on an input error or EOF.
 * Invalid responses leave the interval unchanged and repeat the question.
 */
static int play_game(int use_roman)
{
    int low = GUESS_MIN, high = ROMAN_MAX;
    char left[ROMAN_BUFFER_SIZE], right[ROMAN_BUFFER_SIZE];
    char *answer = NULL;
    size_t capacity = 0;
    const char *yes = _("Yes");
    const char *no = _("No");

    printf(_("Think of an integer between %s and %s.\n"),
           number_text(low, use_roman, left), number_text(high, use_roman, right));
    while (low < high) {
        int middle = low + (high - low) / 2;
        char threshold[ROMAN_BUFFER_SIZE];
        printf(_("Is your number greater than %s? (%s/%s): "),
               number_text(middle, use_roman, threshold), yes, no);
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
        if ((size_t)length == strlen(yes) && strcmp(answer, yes) == 0)
            low = middle + 1;
        else if ((size_t)length == strlen(no) && strcmp(answer, no) == 0)
            high = middle;
        else
            fprintf(stderr, _("Please answer exactly \"%s\" or \"%s\".\n"), yes, no);
    }

    printf(_("Your number is %s.\n"), number_text(low, use_roman, left));
    free(answer);
    return EXIT_SUCCESS;
}

/** Initialize gettext, process options and start the game.
 * @param argc Number of command-line arguments.
 * @param argv Argument vector including the command name.
 * @return EXIT_SUCCESS for help/version or a completed game, otherwise failure.
 */
int main(int argc, char *argv[])
{
    int use_roman = 0;
    int option;

    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    bind_textdomain_codeset(PACKAGE, "UTF-8");
    textdomain(PACKAGE);

    opterr = 0;
    while ((option = getopt_long(argc, argv, "rhV", long_options, NULL)) != -1) {
        switch (option) {
        case 'r': use_roman = 1; break;
        case 'h': print_help(); return EXIT_SUCCESS;
        case 'V': printf("%s %s\n", PROGRAM_NAME, PACKAGE_VERSION); return EXIT_SUCCESS;
        case HELP_MD_OPTION: print_help_md(); return EXIT_SUCCESS;
        default:
            fprintf(stderr, _("Unknown or incomplete option: %s\n"), argv[optind - 1]);
            fprintf(stderr, _("Try '%s --help' for more information.\n"), PROGRAM_NAME);
            return EXIT_FAILURE;
        }
    }
    if (optind != argc) {
        fputs(_("Positional arguments are not supported.\n"), stderr);
        fprintf(stderr, _("Try '%s --help' for more information.\n"), PROGRAM_NAME);
        return EXIT_FAILURE;
    }
    return play_game(use_roman);
}
