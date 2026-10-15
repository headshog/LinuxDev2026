#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int start;
    int stop;
    int step;
    int current;
    int finished;
} range;

static int parse_int(const char *text)
{
    char *end;
    long value;

    errno = 0;
    value = strtol(text, &end, 10);
    if (errno == ERANGE || end == text || *end != '\0' ||
        value < INT_MIN || value > INT_MAX) {
        fprintf(stderr, "Invalid integer: %s\n", text);
        exit(EXIT_FAILURE);
    }
    return (int)value;
}

static void argparse(int argc, char *argv[], int *start, int *stop, int *step)
{
    if (argc == 1) {
        printf("Usage: %s [START] STOP [STEP]\n"
               "Print integers from START (default 0) up to, but not including, STOP.\n"
               "STEP defaults to 1 and must not be zero; negative steps are allowed.\n",
               argv[0]);
        exit(EXIT_SUCCESS);
    }
    if (argc > 4) {
        fprintf(stderr, "Usage: %s [START] STOP [STEP]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    *start = 0;
    *stop = parse_int(argv[1]);
    *step = 1;
    if (argc >= 3) {
        *start = *stop;
        *stop = parse_int(argv[2]);
    }
    if (argc == 4)
        *step = parse_int(argv[3]);
    if (*step == 0) {
        fprintf(stderr, "STEP must not be zero\n");
        exit(EXIT_FAILURE);
    }
}

static void range_init(range *I)
{
    I->current = I->start;
    I->finished = 0;
}

static int range_run(const range *I)
{
    return !I->finished && (I->step > 0 ? I->current < I->stop
                                       : I->current > I->stop);
}

static void range_next(range *I)
{
    /* An overflowing next element is already beyond any representable stop. */
    if ((I->step > 0 && I->current > INT_MAX - I->step) ||
        (I->step < 0 && I->current < INT_MIN - I->step)) {
        I->finished = 1;
        return;
    }
    I->current += I->step;
}

static int range_get(const range *I)
{
    return I->current;
}

int main(int argc, char *argv[]) {
        range I;
        argparse(argc, argv, &I.start, &I.stop, &I.step);
        for(range_init(&I); range_run(&I); range_next(&I))
                printf("%d\n", range_get(&I));
        return 0;
}
