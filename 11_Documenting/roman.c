/** @file roman.c
 * @brief Roman numeral conversion without dynamic allocation.
 */
#include "roman.h"
#include <string.h>

/** A value and its canonical Roman representation. */
struct RomanSymbol {
    int value;             /**< Numeric value of the token. */
    const char *text;      /**< One or two uppercase Roman letters. */
};

/** Tokens in descending order, including all six subtractive pairs. */
static const struct RomanSymbol symbols[] = {
    {1000, "M"}, {900, "CM"}, {500, "D"}, {400, "CD"},
    {100, "C"}, {90, "XC"}, {50, "L"}, {40, "XL"},
    {10, "X"}, {9, "IX"}, {5, "V"}, {4, "IV"}, {1, "I"}
};

int int_to_roman(int value, char *output, size_t capacity)
{
    char encoded[ROMAN_BUFFER_SIZE] = "";

    if (output == NULL || capacity == 0)
        return 0;
    output[0] = '\0';
    if (value < 1 || value > ROMAN_MAX)
        return 0;

    for (size_t i = 0; i < sizeof(symbols) / sizeof(symbols[0]); ++i) {
        while (value >= symbols[i].value) {
            strcat(encoded, symbols[i].text);
            value -= symbols[i].value;
        }
    }
    size_t length = strlen(encoded);
    if (capacity <= length)
        return 0;
    memcpy(output, encoded, length + 1);
    return 1;
}

/** Get the value of an individual Roman letter.
 * @param letter Uppercase Roman letter.
 * @return Its value, or 0 if the letter is not Roman.
 */
static int roman_digit(char letter)
{
    switch (letter) {
    case 'I': return 1;
    case 'V': return 5;
    case 'X': return 10;
    case 'L': return 50;
    case 'C': return 100;
    case 'D': return 500;
    case 'M': return 1000;
    default: return 0;
    }
}

int roman_to_int(const char *text)
{
    char canonical[ROMAN_BUFFER_SIZE];
    int value = 0;

    if (text == NULL)
        return 0;
    size_t length = strlen(text);
    if (length == 0 || length >= ROMAN_BUFFER_SIZE)
        return 0;

    for (size_t i = 0; i < length; ++i) {
        int current = roman_digit(text[i]);
        int next = i + 1 < length ? roman_digit(text[i + 1]) : 0;
        if (current == 0)
            return 0;
        value += current < next ? -current : current;
    }
    if (!int_to_roman(value, canonical, sizeof(canonical)) ||
        strcmp(text, canonical) != 0)
        return 0;
    return value;
}
