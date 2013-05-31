#include <stdlib.h>

#include "random.h"
#include "binary.h"
#include "poly.h"

// TODO: ignore the s rightmost bits


// f: polynomial of degree m
Poly* poly_rand_uniform_poly(const Poly *f) {
  // TODO: receive s as parameter?
  //uint8_t s = (t*W) - binary_degree(f, t); // s = Wt - m
  uint8_t t = f->t;
  //Poly p = malloc(sizeof(uint32_t) * t);
  Poly *p = poly_alloc(t);
  while(t--)
    p->vec[t] = random_uniform_uint32();
  p->vec[0] &= (0xffffffff >> p->s); // align last word
  return p;
}

// f: polynomial of degree m
Poly* poly_rand_bernoulli_poly(const Poly *f, double tau) {
  //uint8_t s = (t*W) - binary_degree(f, t); // s = Wt - m
  uint8_t t = f->t;
  //uint32_t *p = malloc(sizeof(uint32_t) * t);
  Poly *p = poly_alloc(t);
  while(t--)
    p->vec[t] = random_bernoulli_uint32(tau);
  p->vec[0] &= (0xffffffff >> p->s); // align last word
  return p;
}

// t: number of words
// returns c = a + b
Poly* poly_add(const Poly *a, const Poly *b) {
  if(a->t != b->t) {
    return NULL;
  }
  Poly *c = poly_alloc(a->t);
  uint8_t t = a->t;
  c->t = t;
  c->m = a->m;
  c->s = a->s;
  while(t--) {
    c->vec[t] = a->vec[t] ^ b->vec[t];
  }
  return c;
}

// right-to-left comb method
// returns the number of words in C
// C is written to *c. c is allocated here
// TODO: should c be allocated here? becuase we have to return the number of words in C
/*uint8_t poly_mult(const Poly a, const Poly be Poly *c, uint8_t t) {
  // TODO: not sure about this value.. might be just 2*t
  size_t c_words = 2*t - 1; // number of words in C
  uint8_t k, j, actual_j;
  uint8_t i = 0;

  Poly C = *c = poly_alloc(c_words);
  //C = poly_alloc(c_words);
  Poly B = poly_alloc(t);
  for(i = 0; i < t; i++)
    B[i] = b[i];
  for(k = 0; k < W; k++) {
    for(j = 0; j < t; j++) {
      // in our representation, (t - j) - 1 is the same as j in the right-to-left comb method
      actual_j = (t - j) - 1;
      uint8_t kthBit = binary_get_bit(a[actual_j], k);
      
      if(kthBit == 1) {
        // TODO: find a way to not use poly_alloc for newB, but instead use only the B
        Poly newB = poly_alloc(c_words);
        // put the B in newB
        for(i = 0; i < t; i++)
          newB[i + (c_words - t)] = B[i];
        // add j words to newB == shift j
        uint8_t toShift = j;
        while(toShift > 0) {
          poly_shift_left(newB, c_words);
          toShift--;
        }
        // add newB to C. save to tmp in order to free old C
        Poly tmp = poly_add(C, newB, c_words);
        poly_free(C);
        C = tmp;
        // free newB
        poly_free(newB);
      }
    }
    if(k != (W-1)) {
      B = poly_shift_left(B, t);
    }
  }
  // free B
  //poly_free(B);
  // because poly_add creates a new poly, we need to re-set c's address
  *c = C;
  return c_words;
}*/

// NOTE: the Poly a is changed, and the value returned is the same as a
// TODO: return a copy or return the same address?
// addapted from here and from prof: http://stackoverflow.com/questions/2773890/efficient-bitshifting-an-array-of-int
Poly* poly_shift_left(Poly *a) {
  uint8_t i = 0;
  uint8_t t = a->t;
  for(i = 0; i < (t - 1); i++)
    a->vec[i] = (a->vec[i] << 1) | (a->vec[i+1] >> (W - 1));
  a->vec[i] <<= 1;
  return a;
}

Poly* poly_shift_right(Poly *a) {
  uint8_t i = 0;
  uint8_t t = a->t;
  for(i = 0; i < (t - 1); i++)
    a->vec[i] = (a->vec[i] >> 1) | (a->vec[i+1] << (W - 1));
  a->vec[i] >>= 1;
  return a;
}

uint16_t poly_hamming_weight(const Poly *a) {
  uint16_t wt = 0;
  uint8_t t = a->t;
  while(t--)
    wt += binary_hamming_weight(a->vec[t]);
  return wt;
}

// return a mod f
Poly* poly_mod(const Poly *a, const Poly *f) {
  //TODO: implement Algorithm 2.40 or the more efficient one's
  return NULL;
}

// m: degree of the irreducible polynomial
Poly* poly_alloc(uint8_t m) {
  Poly *p = malloc(sizeof(Poly));
  if(p == NULL) // TODO: deal with errors
    return NULL;
  p->m = m;
  p->t = NUMBER_OF_WORDS(m);
  p->s = W*p->t - p->m;
  p->vec = calloc(p->t, sizeof(uint32_t));
  if(p->vec == NULL) {
    free(p);
    return NULL;
  }
  return p;
}

void poly_set_coefs(Poly *p, const uint32_t *coefs) {
  int8_t t = p->t;
  if(p->vec == NULL) {
    p->vec = calloc(p->t, sizeof(uint32_t));
  }
  while(t--) {
    p->vec[t] = coefs[t];
  }
}

void poly_free(Poly *p) {
  if(p != NULL) {
    if(p->vec != NULL)
      free(p->vec);
    free(p);
  }
}
