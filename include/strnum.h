#ifndef STRNUM_H_
#define STRNUM_H_
#include <better_string.h>
#include <macros.h>
#include <stdbool.h>

void init_math();

void math_free();

bool isfloat(string_t *s);

bool isint(string_t *s);

long order(string_t *s);

long string_to_int(string_t *s);

long double string_to_double(string_t *s);

string_t *int_to_string(long d);

string_t *double_to_string(long double f);

string_t *neg(string_t *s);

string_t *ip(string_t *s);

string_t *fp(string_t *s);

bool sum_positive(string_t *m, string_t *n);

// returns the sum of m and n, where:
// m_radix is a pointer to the radix point in m, if known
// n_radix is a pointer to the radix point in n, if known
// sum_buffer has a buffer to put the sum in, if relevant
// sb_radix holds a pointer to the radix point (0th place value) in the buffer, if sum_buffer is not NULL
string_t *sum(string_t *m, string_t *n, char32_t *m_radix, char32_t *n_radix,
              string_t *sum_buffer, char32_t *sb_radix);

string_t *diff(string_t *m, string_t *n);

string_t *product(string_t *m, string_t *n);

string_t *quotient(string_t *m, string_t *n);

#endif // STRNUM_H_
