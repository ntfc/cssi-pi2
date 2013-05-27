#ifndef __POLY_H__
#define __POLY_H__

#include <stdint.h>

typedef uint32_t* Poly;

uint32_t poly_random_uniform_32bit_word();
Poly poly_random_uniform_poly(Poly f, uint8_t t);
uint32_t poly_random_bernoulli_32bit_word(double tau);
Poly poly_random_bernoulli_poly(Poly f, uint8_t t, double tau);
Poly poly_add(Poly a, Poly b, uint8_t t);
Poly poly_mult(Poly a, Poly b, uint8_t t);
Poly poly_mod(Poly a, Poly b, uint8_t t);

void poly_free(Poly p);
#endif
