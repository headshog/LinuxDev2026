#define _POSIX_C_SOURCE 200809L

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

enum {
        ESCAPE = 27,
        MIN_LINES = 3,
        MIN_COLS = 5
};

static void print_line(WINDOW *window, int row, const char *line,
                       size_t length, int width)
{
        while (length > 0 && (line[length - 1] == '\n' ||
                              line[length - 1] == '\r'))
                --length;

        if (length > (size_t)width)
                length = (size_t)width;

        (void)mvwaddnstr(window, row, 0, line, (int)length);
}

int main(int argc, char *argv[])
{
        FILE *input;
        WINDOW *frame;
        WINDOW *text;
        char *line = NULL;
        size_t capacity = 0;
        ssize_t length;
        int text_height;
        int text_width;
        int row;
        int key;

        if (argc != 2) {
                fprintf(stderr, "Usage: %s FILE\n", argv[0]);
                return EXIT_FAILURE;
        }

        input = fopen(argv[1], "r");
        if (input == NULL) {
                perror(argv[1]);
                return EXIT_FAILURE;
        }

        if (initscr() == NULL) {
                fprintf(stderr, "Cannot initialize ncurses\n");
                fclose(input);
                return EXIT_FAILURE;
        }

        if (LINES < MIN_LINES || COLS < MIN_COLS) {
                endwin();
                fprintf(stderr, "Terminal is too small\n");
                fclose(input);
                return EXIT_FAILURE;
        }

        noecho();
        cbreak();
        curs_set(0);

        frame = newwin(LINES, COLS, 0, 0);
        text_height = LINES - 2;
        text_width = COLS - 3;
        text = newwin(text_height, COLS - 2, 1, 1);
        if (frame == NULL || text == NULL) {
                if (text != NULL)
                        delwin(text);
                if (frame != NULL)
                        delwin(frame);
                endwin();
                fprintf(stderr, "Cannot create ncurses windows\n");
                fclose(input);
                return EXIT_FAILURE;
        }

        box(frame, 0, 0);
        (void)mvwaddnstr(frame, 0, 2, argv[1], COLS - 4);
        wrefresh(frame);

        scrollok(text, TRUE);
        for (row = 0; row < text_height; ++row) {
                length = getline(&line, &capacity, input);
                if (length < 0)
                        break;
                print_line(text, row, line, (size_t)length, text_width);
        }
        wrefresh(text);

        while ((key = wgetch(text)) != ESCAPE) {
                if (key != ' ')
                        continue;

                length = getline(&line, &capacity, input);
                if (length < 0)
                        continue;

                wscrl(text, 1);
                wmove(text, text_height - 1, 0);
                wclrtoeol(text);
                print_line(text, text_height - 1, line, (size_t)length,
                           text_width);
                wrefresh(text);
        }

        free(line);
        delwin(text);
        delwin(frame);
        endwin();
        fclose(input);
        return EXIT_SUCCESS;
}
