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
    fprintf(stderr, "ERROR poly_mult\n");
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
      if(binary_get_bit(a->vec[GET_VEC_WORD_INDEX(a->t, j)], k) == 1) {
        i = j;
        // C{j} = C{j} + B
        while((i < c_words) && ((i - j) <= t)) {
          // C[i] = C[i] ^ B[i - j]
          c->vec[GET_VEC_WORD_INDEX(c_words, i)] ^= B[GET_VEC_WORD_INDEX(b->t + 1, i - j)];
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

// TODO: return in case of error. Use errno
uint8_t poly_get_bit(const Poly *a, uint32_t b) {
  if(b >= (a->t * W)) {
    fprintf(stderr, "ERROR: bit %u is out of range (max: %u - 1)\n", b, (a->t * W));
    return 2;
  }
  uint16_t word = GET_WORD_FROM_BIT(b);
  uint8_t bit = b % W; // bit position in the word
  return binary_get_bit(a->vec[GET_VEC_WORD_INDEX(a->t, word)], bit);
}

Poly* poly_get_r(const Poly *f) {
  // TODO: f must be irreducible
  uint16_t i = 0, j = 0;
  // position of the MSB set in f->vec
  uint16_t m = binary_degree(f->vec[0]);
  
  uint32_t first_r_word = f->vec[0] ^ (1 << m);
  
  Poly *r = NULL;
  if(first_r_word != 0) {
    // r has f->t words
    r = poly_clone(f, f->t);
    r->vec[0] = first_r_word;
  }
  else {
    // traverse f->vec until we find the last word of r
    for(i = 1; i < f->t && (f->vec[i] == 0); i++)
      ;
    // get first word of r
    m = binary_degree(f->vec[i]);
    // create poly = r(x)
    r = poly_alloc(m, f->t - i);
    for(j = i; j < f->t; j++) {
      r->vec[j - i] = f->vec[j];
    }
  }
  return r;
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
    T = a->vec[GET_VEC_WORD_INDEX(a->t, i)];
    printf("i = %u, a->t = %u\n", i, a->t);
    printf("%u\n", GET_VEC_WORD_INDEX(a->t, i - 8));
    // reduction modulo x^233 + x^74 + 1
    // C[i-8]
    /*word_index = GET_VEC_WORD_INDEX(a->t, i - 8);
    a->vec[word_index] = a->vec[word_index] ^ (T << 23);
    // C[i-7]
    word_index = GET_VEC_WORD_INDEX(a->t, i - 7);
    a->vec[word_index] = a->vec[word_index] ^ (T >> 9);
    // C[i-5]
    word_index = GET_VEC_WORD_INDEX(a->t, i - 5);
    a->vec[word_index] = a->vec[word_index] ^ (T << 1);
    // C[i-4]
    word_index = GET_VEC_WORD_INDEX(a->t, i - 4);
    a->vec[word_index] = a->vec[word_index] ^ (T >> 31);*/
  }
  /*T = a->vec[GET_VEC_WORD_INDEX(a->t, 7)] >> 9;
  // C[0]
  word_index = GET_VEC_WORD_INDEX(a->t, 0);
  a->vec[word_index] = a->vec[word_index] ^ T;
  // C[2]
  word_index = GET_VEC_WORD_INDEX(a->t, 2);
  a->vec[word_index] = a->vec[word_index] ^ (T << 10);
  // C[3]
  word_index = GET_VEC_WORD_INDEX(a->t, 3);
  a->vec[word_index] = a->vec[word_index] ^ (T >> 22);
  // C[7]
  word_index = GET_VEC_WORD_INDEX(a->t, 7);
  a->vec[word_index] = a->vec[word_index] & 0x1FF;
  
  a->t /= 2;
  a->m /= 2;
  a->s = W*a->t - a->m;
  
  uint32_t *new_poly = calloc(a->t, sizeof(uint32_t));
  for(i = 0; i < a->t; i++) {
    new_poly[GET_VEC_WORD_INDEX(a->t, i)] = a->vec[GET_VEC_WORD_INDEX((a->t * 2), i)];
  }
  a->vec = new_poly;*/
  
  return a;
}

Poly* poly_mod(const Poly *c, const Poly *f, uint32_t ***table) {
  if(*table == NULL) {
    // pre-compute table
    *table = poly_compute_mod_table(f);
  }
  
  uint16_t j, i;
  uint8_t k;
  uint16_t m = f->m;
  uint16_t j_aux;
  
  // TODO: validate c->t and c->m  
  Poly *C = poly_alloc(c->m , c->t);
  
  // copy c->vec to C->vec
  for(i = 0; i < c->t; i++) {
    C->vec[i] = c->vec[i];
  }
  
  for(i = (2 * f->m) - 2; i >= f->m; i--) {
    uint8_t ci = poly_get_bit(C, i);
    if(ci == 1) {
      j = floor(((double)i - (double)m) / (double)W);
      k = (i - m) - (W * j);
      
      j_aux = j;
      while( (j_aux < C->t) && ((j_aux - j) <= f->t)) {
         // C{j_aux} = C{j_aux} ^ u[k]
        uint16_t ci_i = GET_VEC_WORD_INDEX(C->t, j_aux);
        uint32_t ci = C->vec[ci_i];
        
        uint32_t uk_k = GET_VEC_WORD_INDEX(f->t+1, j_aux - j);
        uint32_t uk = (*table)[k][uk_k];
        // C[j_aux] = C[j_aux] ^ u[k][j_aux - j]
        C->vec[ci_i] = ci ^ uk;
        
        j_aux++;
      }
    }
    
  }

  Poly *cMod = poly_alloc(f->m, f->t);
  for(i = 0; i < cMod->t; i++) {
    cMod->vec[(cMod->t - 1) - i] = C->vec[(C->t - 1) - i];
  }
  // not necessary
  //cMod->vec[0] &= (0xffffffff >> cMod->s); // align last word
  
  poly_free(C);
  return cMod;
}

// this table uses f(x) instead of r(x)
// TODO: use r(x) instead of f(x)!!!!
uint32_t** poly_compute_mod_table(const Poly *f) {
  uint8_t k = 0;
  uint16_t i = 0;
  uint32_t **table; // 2-dimension array = table
  // one more word than f
  uint16_t t = f->t + 1; // number of words in each u_k
  table = malloc(sizeof(uint32_t*) * W);
  
  uint32_t *u0 = calloc(t, sizeof(uint32_t));
  // copy f->vec to u0. u0[0] = 0x0, u0[1] = f->vec[0], etc
  for(i = 1; i < t; i++) {
    u0[i] = f->vec[i-1];
  }
  table[0] = u0;
  k = 1;
  while(k < W) {
    table[k] = calloc(t, sizeof(uint32_t));
    binary_array_shift_left2(table[k - 1], t, table[k]);
    k++;
  }
  
  // print table  
  /*for(k = 0; k < W; k++) {
    printf("u[%u] = ", k);
    for(i = 0; i < t; i++) {
      printf("0x%.8x ", table[k][i]);
    }
    printf("\n");
  }*/
  return table;
}

// t: length of table
void poly_free_table(uint32_t **t, uint8_t n) {
  uint8_t i = 0;
  
  if(t != NULL) {
    
    for(i = 0; i < n; i++) {
      if(t[i] != NULL) {
        free(t[i]);
      }
    }
    free(t);
  }
  
}
