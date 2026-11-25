#include "roman.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        return EXIT_FAILURE; \
    } \
} while (0)

int main(void)
{
    const char *thousands[] = {"", "M", "MM", "MMM"};
    const char *hundreds[] = {"", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"};
    const char *tens[] = {"", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"};
    const char *ones[] = {"", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"};
    char actual[ROMAN_BUFFER_SIZE], expected[ROMAN_BUFFER_SIZE];

    for (int value = 1; value <= ROMAN_MAX; ++value) {
        int length = snprintf(expected, sizeof(expected), "%s%s%s%s",
                              thousands[value / 1000], hundreds[value / 100 % 10],
                              tens[value / 10 % 10], ones[value % 10]);
        CHECK(length > 0 && (size_t)length < sizeof(expected));
        CHECK(int_to_roman(value, actual, sizeof(actual)));
        CHECK(strcmp(actual, expected) == 0);
        CHECK(roman_to_int(actual) == value);
    }

    const char *invalid[] = {"", "0", "IIII", "IIV", "VX", "IC", "XM", "MMMM",
                             "iv", " I", "I ", "ABC", "MCMC", "IL", "IVIV",
                             "MMMDCCCLXXXVIIII", "MMMMMMMMMMMMMMMM"};
    for (size_t i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i)
        CHECK(roman_to_int(invalid[i]) == 0);
    CHECK(roman_to_int(NULL) == 0);
    CHECK(!int_to_roman(0, actual, sizeof(actual)) && actual[0] == '\0');
    CHECK(!int_to_roman(-1, actual, sizeof(actual)) && actual[0] == '\0');
    CHECK(!int_to_roman(4000, actual, sizeof(actual)) && actual[0] == '\0');
    CHECK(!int_to_roman(1, NULL, ROMAN_BUFFER_SIZE));
    actual[0] = 'x';
    CHECK(!int_to_roman(1, actual, 0) && actual[0] == 'x');
    CHECK(!int_to_roman(1, actual, 1) && actual[0] == '\0');
    CHECK(!int_to_roman(3888, actual, ROMAN_BUFFER_SIZE - 1));
    CHECK(actual[0] == '\0');
    CHECK(int_to_roman(3888, actual, sizeof(actual)) && strlen(actual) == 15);
    puts("All 3999 Roman conversions, invalid input and buffer limits: OK");
    return EXIT_SUCCESS;
}
