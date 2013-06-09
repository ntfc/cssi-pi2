#include <stdlib.h>
#include <stdio.h>
#include <math.h> // for ceil and floor

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
  //uint16_t c_words = CEILING((double)c_max_degree / (double)W); // number of words in C
  uint16_t c_words = (uint16_t)ceil((double)c_max_degree / (double)W); // number of words in C
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

Poly* poly_mod(const Poly *a, const Poly *f) {
  // TODO: validate a->t and a->m
  uint16_t i, i_aux;
  uint8_t j, k, bit, j_aux;
  uint16_t word;
  
  uint32_t *C = calloc(a->t, sizeof(uint32_t));
  // copy a->vec to C
  for(i = 0; i < a->t; i++) {
    C[i] = a->vec[i];
  }
  
  
  ////////////////////////// start pre-computation
  uint32_t *(u[W]);
  // calculate array
  for(i = 0; i < W; i++) {
    if(i == 0) {
      // alloc for u[0] = f->vec[0] + 1 word
      u[0] = (uint32_t*)calloc(f->t + 1, sizeof(uint32_t));
      //printf("u[0] = 0x%.8x ", u[0][0]);
      // copy f->vec to u[0]
      for(i_aux = 1; i_aux <= f->t; i_aux++) {
        u[0][i_aux] = f->vec[i_aux - 1];
        //printf("0x%.8x ", u[i][i_aux]);
      }
      //printf("\n");
    }
    else {
      // alloc for u[i] = f->vec[i] << i
      u[i] = calloc(f->t + 1, sizeof(uint32_t));
      // ATTENTION: u[i] has one more word than f->vec
      for(i_aux = 0; i_aux <= f->t; i_aux++) {
        u[i][i_aux] = u[i - 1][i_aux];
      }
      //binary_array_shift_left(u[i], f->t + 1);
      // TODO: why? do the shift here, otherwise valgrind crashes
      int16_t ii = 0;
      for(ii = 0; ii < ((f->t + 1) - 1); ii++)
        u[i][ii] = (u[i][ii] << 1) | (u[i][ii+1] >> (W - 1));
      u[i][ii] <<= 1;
      /*// prints
      printf("u[%u] = ", i);
      for(i_aux = 0; i_aux <= f->t; i_aux++) {
        printf("0x%.8x ", u[i][i_aux]);
      }
      printf("\n");*/
    }
  }
  ////////////////////////// end of pre-computation
  printf("C = ");
  for(i = 0; i < a->t; i++) {
    printf("0x%.8x ", C[i]);
  }
  printf("\n");
  for(i = (2 * f->m) - 2; i >= f->m; i--) {
    word = floor((double)i / (double)W);
    bit = W - (i - (word * W)) - 1;
    if(binary_get_bit(C[GET_WORD_INDEX(a->t, word)], bit) == 1) {
      j = floor((double)(i - f->m) / (double)W);
      k = (i - f->m) - (W * j);
      j_aux = j;
      while( (j_aux < a->t) && ((j_aux - j) <= f->t)) {
        // C{j_aux} = C{j_aux} ^ u[k]
        C[GET_WORD_INDEX(a->t, j_aux)] ^= u[k][GET_WORD_INDEX(f->t + 1, j_aux - j)];
        j_aux++;
      }
    }
  }
  printf("\n");
  
  printf("C = ");
  for(i = 0; i < a->t; i++) {
    printf("0x%.8x ", C[i]);
  }
  printf("\n");
  //free(C);
  
  /*Poly *b = poly_alloc(f->m, f->t);
  
  for(i = 0; i < b->t; i++) {
    b->vec[i] = C[(f->t - i) - 1];
  }
  //b->vec[0] &= (0xffffffff >> b->s);*/
  return a;
}
