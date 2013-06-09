#include <stdlib.h>
#include <stdio.h>

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

// returns c = a + b
Poly* poly_add(const Poly *a, const Poly *b) {
  if(a->t != b->t) {
    fprintf(stderr, "ERROR poly_add\n");
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

// right-to-left comb method
Poly* poly_mult(const Poly *a, const Poly *b) {
  if(a->m != a->m || a->t != b->t) {
    fprintf(stderr, "ERRO poly_mult\n");
    return 0;
  }
  uint16_t j, i;
  uint8_t k;
  uint16_t t = a->t;
  uint16_t m = a->m;
  uint16_t c_max_degree = 2*m - 1; // C is of degree at most m-1
  uint16_t c_words = CEILING((double)c_max_degree / (double)W); // number of words in C
  //uint8_t c_unused = W*c_words - c_max_degree;  
  Poly *c = poly_alloc(c_max_degree, c_words);
  // B is a copy of b, but with one more word
  uint32_t *B = calloc(b->t + 1, sizeof(uint32_t));
  // b[0] is already zero'd
  for(j = 0; j < b->t; j++) {
    B[j+1] = b->vec[j];
  }
  for(k = 0; k < W; k++) {
    for(j = 0; j < t; j++) {
      // get k-th bit of A[j]
      if(binary_get_bit(a->vec[GET_WORD_INDEX(a->t, j)], k) == 1) {
        i = j;
        // C{j} = C{j} + B
        while((i < c_words) && ((i - j) <= t)) {
          // C[i] = C[i] ^ B[i - j]
          c->vec[GET_WORD_INDEX(c_words, i)] ^= B[GET_WORD_INDEX(b->t + 1, i - j)];
          i++;
        }
      }
    }
    if(k != W - 1) {
      // B = B * x
      binary_array_shift_left(B, t+1);
    }
  }
  // free B
  free(B);
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

// clone poly p, but use new_t words instead of a->t
Poly* poly_clone(const Poly *p, uint16_t new_t) {
  Poly *c = poly_alloc(p->m, new_t);
  uint16_t i = 0;
  for(i = 0; i < p->t; i++)
    c->vec[i + (new_t - p->t)] = p->vec[i];
  return c;
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

Poly* poly_create_poly_from_coeffs(const Poly *f, const uint16_t *v, uint8_t n) {
  Poly *p = poly_alloc(f->m, f->t);
  uint16_t word;
  uint8_t pos;
  while(n--) {
    word = v[n] / W; // word to xor with. NOTE: word=0 <=> p->vec[p->t-1]
    pos = v[n] - (word * W); // bit position to xor
    // why does this work?
    //p->vec[(p->t - 1) - word] ^= (0x1 << v[n]); // actual xor
    p->vec[(p->t - 1) - word] ^= (0x1 << pos); // actual xor
  }
  return p;
}

// returns only the first word
uint32_t poly_get_r(const Poly *a) {
  uint32_t i = W;
  while(i != 0 && binary_get_bit(a->vec[0], i) != 1)
    i--;
  return (1 << i) ^ a->vec[0];
}



Poly* poly_fast_mod_reduction(Poly *a) {
  // TODO: validate a->t and a->m!!!!
  int16_t i = 0;
  uint32_t T = 0;
  uint16_t word_index;

  for(i = a->t; i > (a->t / 2); i--) {
    T = a->vec[GET_WORD_INDEX(a->t, i)];
    printf("i = %u, a->t = %u\n", i, a->t);
    printf("%u\n", GET_WORD_INDEX(a->t, i - 8));
    // reduction modulo x^233 + x^74 + 1
    // C[i-8]
    /*word_index = GET_WORD_INDEX(a->t, i - 8);
    a->vec[word_index] = a->vec[word_index] ^ (T << 23);
    // C[i-7]
    word_index = GET_WORD_INDEX(a->t, i - 7);
    a->vec[word_index] = a->vec[word_index] ^ (T >> 9);
    // C[i-5]
    word_index = GET_WORD_INDEX(a->t, i - 5);
    a->vec[word_index] = a->vec[word_index] ^ (T << 1);
    // C[i-4]
    word_index = GET_WORD_INDEX(a->t, i - 4);
    a->vec[word_index] = a->vec[word_index] ^ (T >> 31);*/
  }
  /*T = a->vec[GET_WORD_INDEX(a->t, 7)] >> 9;
  // C[0]
  word_index = GET_WORD_INDEX(a->t, 0);
  a->vec[word_index] = a->vec[word_index] ^ T;
  // C[2]
  word_index = GET_WORD_INDEX(a->t, 2);
  a->vec[word_index] = a->vec[word_index] ^ (T << 10);
  // C[3]
  word_index = GET_WORD_INDEX(a->t, 3);
  a->vec[word_index] = a->vec[word_index] ^ (T >> 22);
  // C[7]
  word_index = GET_WORD_INDEX(a->t, 7);
  a->vec[word_index] = a->vec[word_index] & 0x1FF;
  
  a->t /= 2;
  a->m /= 2;
  a->s = W*a->t - a->m;
  
  uint32_t *new_poly = calloc(a->t, sizeof(uint32_t));
  for(i = 0; i < a->t; i++) {
    new_poly[GET_WORD_INDEX(a->t, i)] = a->vec[GET_WORD_INDEX((a->t * 2), i)];
  }
  a->vec = new_poly;*/
  
  return a;
}

Poly* poly_mod(Poly *a, const Poly *f) {
  uint16_t i = 0;
  uint16_t m = f->m;
  uint8_t k = 0;
  Poly *r = poly_clone(f, f->t);
  // obtain r(x) such that f(x) = x^m + r(x) 
  r->vec[0] = r->vec[0] ^ (1 << f->m);
  printf("r(x)="); poly_print_poly(r);
  Poly *u[W];
  Poly *c;

  // pre computation
  u[0] = poly_clone(f, a->t);
  for(k = 1; k < W; k++) {
    u[k] = poly_clone(u[k-1], a->t);
    u[k] = poly_shift_left(u[k]);
  }
    
  // reduction
  for(i = 2*m; i > m; i--) {
    //uint16_t word = i/W; // NOTE: word =  0 <=> p->vec[p->t-1]
    uint16_t word = (a->t - 1) - i/W;
    uint16_t pos = i - (word * W);
    uint8_t ci = binary_get_bit(a->vec[word], pos);
    if(ci == 1) {
      uint16_t j = (i - m) / W;
      k = (i - m) - W*j;
      uint16_t toShift = j * W;
      while(toShift > 0) {
        u[k] = poly_shift_left(u[k]);
        toShift--;
      }
      // c(x) = c(x) + u_k(x)*x^{j*W}
      c = poly_add(a, u[k]);
      poly_free(a);
      a = c;
    }
  }
  
  // frees
  for(k = 0; k < W; k++)
    poly_free(u[k]);
  
  // delete unnecessary bits
  Poly *d = poly_alloc(f->m, f->t);
  for(i = 0; i < f->t; i++) {
    d->vec[i] = a->vec[d->t + i];
  }
  d->vec[0] &= (0xffffffff >> d->s);
  poly_free(a);
  return d;
}
