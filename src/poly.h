#ifndef __POLY_H__
#define __POLY_H__

#include <stdint.h>

// returns the i-th word from an array with t-words, according to our representation
// ie A[1] <=> a->vec[(t - 1) - 1]
#define GET_VEC_WORD_INDEX(t,i) ( (( t ) - ( i )) - 1 )

// given the number of words in a poly and the desired bit to be known, return the word index
// ie: get GET_WORD_FROM_BIT(7) = 1 <=> a->vec[GET_WORD_INDEX(t, GET_WORD_FROM_BIT)]
#define GET_WORD_FROM_BIT(i) ( floor( (double)(i) / (double)(W)) )


// TODO: more polynomial info
typedef struct s_poly {
  uint16_t t;
  uint8_t s;
  uint16_t m;
  uint32_t *vec; // actual polynomial, with t words
} Poly;

typedef Poly* PolyVec;

Poly* poly_rand_uniform_poly(const Poly *f);
Poly* poly_rand_bernoulli_poly(const Poly *f, double tau);

Poly* poly_add(const Poly *a, const Poly *b);
uint16_t poly_degree(const Poly *p);
// TODO: poly_mult_scalar(const Poly *a, uint32_t s); ??
Poly* poly_mult(const Poly *a, const Poly *b);
// return c mod f. does not change value of c
Poly* poly_mod(const Poly *c, const Poly *f, uint32_t ***table);
// fast mod reduction
Poly* poly_fast_mod_reduction(Poly *a);
uint16_t poly_hamming_weight(const Poly *a);

Poly* poly_shift_left(Poly *a);
Poly* poly_shift_right(Poly *a);

Poly* poly_alloc(uint16_t m);
// TODO: careful with this one!!
void poly_set_coeffs_from_uint32(Poly *p, const uint32_t *coefs);
void poly_free(Poly *p);

// n: length of v
Poly* poly_create_poly_from_coeffs(const Poly *f, const uint16_t *v, uint8_t n);

Poly* poly_create_irreduc(uint16_t m, const uint16_t* r, uint8_t n);

Poly* poly_clone(const Poly *p, uint16_t new_t);

uint8_t poly_get_bit(const Poly *a, uint32_t b);

Poly* poly_add_degree(const Poly *a, uint16_t add);

uint32_t** poly_compute_mod_table(const Poly *f);

// t: length of table
void poly_free_table(uint32_t **t, uint8_t n);

#endif
