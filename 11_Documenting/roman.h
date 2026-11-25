/** @file roman.h
 * @brief Strict conversion between integers and canonical Roman numerals.
 */
#ifndef ROMAN_H
#define ROMAN_H

#include <stddef.h>

/** Largest supported integer; zero and negative values have no representation. */
#define ROMAN_MAX 3999

/** Buffer size including NUL: the longest numeral is MMMDCCCLXXXVIII. */
#define ROMAN_BUFFER_SIZE 16

/** Convert an integer to uppercase Roman notation, using IV, IX, XL, XC, CD, CM.
 * @param value Integer in the range 1 through ROMAN_MAX.
 * @param output Destination buffer, or NULL to report an error.
 * @param capacity Size of output in bytes, including room for a terminating NUL.
 * @return 1 on success, 0 on invalid input or insufficient capacity.
 * On failure, a non-NULL output with positive capacity is set to an empty string.
 */
int int_to_roman(int value, char *output, size_t capacity);

/** Parse a canonical uppercase Roman numeral without spaces.
 * @param text NUL-terminated input string; NULL is also rejected safely.
 * @return Integer in 1..ROMAN_MAX, or 0 for invalid input.
 * Lowercase, noncanonical forms such as IIII or IC, and values above
 * ROMAN_MAX are rejected by comparison with the canonical representation.
 */
int roman_to_int(const char *text);

#endif
