#include <stdlib.h>

#include "random.h"
#include "binary.h"
#include "poly.h"

// TODO: ignore the s rightmost bits


// f: polynomial of degree m
Poly* poly_rand_uniform_poly(const Poly *f) {
  // TODO: receive s as parameter?
  //uint8_t s = (t*W) - binary_degree(f, t); // s = Wt - m
  uint16_t t = f->t;
  //Poly p = malloc(sizeof(uint32_t) * t);
  Poly *p = poly_alloc(f->m, t);
  while(t--)
    p->vec[t] = random_uniform_uint32();
  p->vec[0] &= (0xffffffff >> p->s); // align last word
  return p;
}

// f: polynomial of degree m
Poly* poly_rand_bernoulli_poly(const Poly *f, double tau) {
  //uint8_t s = (t*W) - binary_degree(f, t); // s = Wt - m
  uint16_t t = f->t;
  //uint32_t *p = malloc(sizeof(uint32_t) * t);
  Poly *p = poly_alloc(f->m, t);
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
  Poly *c = poly_alloc(a->m, a->t);
  uint16_t t = a->t;
  c->t = t;
  c->m = a->m;
  c->s = a->s;
  while(t--) {
    c->vec[t] = a->vec[t] ^ b->vec[t];
  }
  return c;
}

// TODO: reorganize this. poly_alloc is being called wrong
// right-to-left comb method
Poly* poly_mult(const Poly *a, const Poly *b) {
  if(a->t != b->t) {
    return 0;
  }
  uint16_t t = a->t;
  uint16_t m = a->m;
  size_t c_words = 2*t; // number of words in C
  size_t c_max_deg = 2*m; // C is of degree at most m-1
  uint8_t k, j, actual_j;
  uint8_t i = 0;

  Poly *c = poly_alloc(c_max_deg, c_words);
  //C = poly_alloc(c_words);
  Poly *B = poly_alloc(m, t);
  for(i = 0; i < t; i++)
    B->vec[i] = b->vec[i];
  for(k = 0; k < W; k++) {
    for(j = 0; j < t; j++) {
      // in our representation, (t - j) - 1 is the same as j in the right-to-left comb method
      actual_j = (t - j) - 1;
      uint8_t kthBit = binary_get_bit(a->vec[actual_j], k);
      
      if(kthBit == 1) {
        // TODO: find a way to not use poly_alloc for newB, but instead use only the B
        Poly *newB = poly_alloc(c_max_deg, c_words);
        // put the B in newB
        for(i = 0; i < t; i++)
          newB->vec[i + (c_words - t)] = B->vec[i];
        
        // add j words to newB == shift j
        uint16_t toShift = j;
        while(toShift > 0) {
          newB = poly_shift_left(newB);
          toShift--;
        }
        // add newB to C. save to tmp in order to free old C
        Poly *tmp = poly_add(c, newB);
        poly_free(c);
        c = tmp;
        // free newB
        poly_free(newB);
      }
    }
    if(k != (W-1)) {
      B = poly_shift_left(B);
    }
  }
  // free B
  poly_free(B);
  return c;
}

// NOTE: the Poly a is changed, and the value returned is the same as a
// TODO: return a copy or return the same address?
// addapted from here and from prof: http://stackoverflow.com/questions/2773890/efficient-bitshifting-an-array-of-int
Poly* poly_shift_left(Poly *a) {
  uint16_t i = 0;
  uint16_t t = a->t;
  // TODO: use this: binary_array_shift_left(a->vec, t);
  for(i = 0; i < (t - 1); i++)
    a->vec[i] = (a->vec[i] << 1) | (a->vec[i+1] >> (W - 1));
  a->vec[i] <<= 1;
  return a;
}

Poly* poly_shift_right(Poly *a) {
  uint16_t i = 0;
  uint16_t t = a->t;
  for(i = 0; i < (t - 1); i++)
    a->vec[i] = (a->vec[i] >> 1) | (a->vec[i+1] << (W - 1));
  a->vec[i] >>= 1;
  return a;
}

uint16_t poly_hamming_weight(const Poly *a) {
  uint16_t wt = 0;
  uint16_t t = a->t;
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
// t: number of words to allocate
Poly* poly_alloc(uint16_t m, uint16_t t) {
  Poly *p = malloc(sizeof(Poly));
  if(p == NULL) // TODO: deal with errors
    return NULL;
  p->m = m;
  p->t = t;
  p->s = W*p->t - p->m;
  p->vec = calloc(p->t, sizeof(uint32_t));
  if(p->vec == NULL) {
    free(p);
    return NULL;
  }
  return p;
}

// coefs: array representing the coefficients
// note: allocates a new p->vec only if necessary
void poly_set_coeffs_from_uint32(Poly *p, const uint32_t *coefs) {
  int16_t t = p->t;
  // TODO: validate coeffs, ie, if degree >= m, truncate it
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

uint16_t poly_degree(const Poly *p) {
  return binary_degree(p->vec[0]) + (W * (p->t-1));
}

// v: ordered from smallest to biggest
Poly* poly_create_poly_from_coeffs(const Poly *f, const uint16_t *v, uint8_t n) {
  Poly *p = poly_alloc(f->m, f->t);
  uint16_t toShift = 0;
  uint16_t word;
  uint8_t pos;
  while(n--) {
    word = v[n] / W; // word to xor with. NOTE: word=0 <=> p->vec[p->t-1]
    pos = v[n] - (word * W); // bit position to xor
    p->vec[(p->t - 1) - word] ^= (0x1 << v[n]); // actual xor
  }
  return p;
}
