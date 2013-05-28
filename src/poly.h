#ifndef __POLY_H__
#define __POLY_H__

#include <stdint.h>

typedef uint32_t* Poly;

uint32_t poly_random_uniform_32bit_word();
Poly poly_random_uniform_poly(const Poly f, uint8_t t);
uint32_t poly_random_bernoulli_32bit_word(double tau);
Poly poly_random_bernoulli_poly(const Poly f, uint8_t t, double tau);
Poly poly_add(const Poly a, const Poly b, uint8_t t);
uint8_t poly_mult(const Poly a, const Poly b, Poly *c, uint8_t t);
Poly poly_mod(Poly a, Poly b, uint8_t t);

uint16_t poly_hamming_weight(const Poly a, uint8_t t);

Poly poly_shift_left(Poly a, uint8_t t);

void poly_free(Poly p);

// TODO: delete this
void poly_print_poly(const Poly f, uint8_t t);
#endif
