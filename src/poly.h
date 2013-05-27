#ifndef __POLY_H__
#define __POLY_H__

#include <stdint.h>

typedef uint32_t* Poly;

uint32_t random_uniform_32bit_word();
Poly random_uniform_poly(uint8_t t);
uint32_t random_bernoulli_32bit_word(double tau);
uint32_t* gen_bernoulli_rand_poly(uint32_t t, double tau);
#endif
