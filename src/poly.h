#ifndef __POLY_H__
#define __POLY_H__

#include <stdint.h>

// TODO: more polynomial info
typedef struct s_poly {
  uint8_t t;
  uint8_t s;
  uint8_t m;
  uint32_t *vec; // actual polynomial, with t words
} Poly;

Poly* poly_rand_uniform_poly(const Poly *f);
Poly* poly_rand_bernoulli_poly(const Poly *f, double tau);

Poly* poly_add(const Poly *a, const Poly *b);
uint16_t poly_degree(const Poly *p);
uint8_t poly_mult(const Poly *a, const Poly *b, Poly *c);
// return a mod f
Poly* poly_mod(const Poly *a, const Poly *f);

uint16_t poly_hamming_weight(const Poly *a);

Poly* poly_shift_left(Poly *a);
Poly* poly_shift_right(Poly *a);

Poly* poly_alloc(uint8_t m);
void poly_free(Poly *p);

#endif
