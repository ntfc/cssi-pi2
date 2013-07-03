#ifndef __POLY_H__
#define __POLY_H__

#include <stdint.h>

typedef struct s_poly {
  uint32_t *vec;
  uint16_t n_words; // length of vec
} Poly;

typedef struct s_poly_crt {
  uint8_t m;
  Poly **crt; // size = m
} PolyCRT;

// returns the i-th word from an array with t-words, according to our representation
// ie A[1] <=> a->vec[(t - 1) - 1]
#define GET_VEC_WORD_INDEX(nwords,index) ( (( nwords ) - ( index )) - 1 )

// find in wich word the bit i is (from right to left)
#define BIT_WORD(i) ( floor( (double)(i) / (double)(W)) )

/* Functions */
Poly *poly_alloc(uint16_t t);
void poly_free(Poly *p);
Poly *poly_clone(const Poly *p);
uint32_t poly_degree(const Poly *p);
Poly *poly_refresh(Poly *p);
Poly* poly_rand_uniform_poly(uint32_t m);
Poly* poly_rand_bernoulli_poly(uint32_t m, double tau);
uint8_t poly_get_bit(const Poly *a, uint32_t pos);
uint16_t poly_hamming_weight(const Poly *a);
Poly *poly_get_r(const Poly *p);
uint32_t *poly_vec_realloc(uint32_t *v, uint16_t t, uint16_t nmemb);
uint32_t** table_compute_mod_table(const Poly *f);
void table_free(uint32_t **t);
Poly* poly_shift_left(Poly *a, uint16_t i);
Poly* poly_shift_right(Poly *a, uint16_t i);
Poly* poly_add(const Poly *a, const Poly *b);
Poly* poly_mult(const Poly *a, const Poly *b);
Poly* poly_mod(const Poly *c, const Poly *f);

/* CRT */

PolyCRT* poly_crt_alloc(uint8_t m);
void poly_crt_free(PolyCRT *p);
PolyCRT *poly_crt_rand_uniform(uint16_t nmemb, const PolyCRT *deg);
PolyCRT* poly_crt_rand_bernoulli(uint32_t deg, const PolyCRT *f, double tau);
PolyCRT* poly_to_crt(const Poly *a, const PolyCRT *f);
Poly *poly_crt_to_poly(const PolyCRT *a);
PolyCRT *poly_crt_add(const PolyCRT *a, const PolyCRT *b);
PolyCRT* poly_crt_mult(const PolyCRT *a, const PolyCRT *b, const PolyCRT *f);

#endif
