#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "poly.h"
#include "binary.h"
#include "random.h"

Poly *poly_alloc(uint16_t t) {
  Poly *p = malloc(sizeof(Poly));
  if(!p) {
    fprintf(stderr, "ERROR: alloc error\n");
    return NULL;
  }
  p->n_words = t;
  p->vec = calloc(t, sizeof(uint32_t));
  if(!p->vec) {
    fprintf(stderr, "ERROR: alloc error\n");
    free(p);
    return NULL;
  }
  return p;
}
void poly_free(Poly *p) {
  if(p) {
    if(p->vec) {
      free(p->vec);
    }
    free(p);
  }
}

Poly *poly_clone(const Poly *p) {
  Poly *a = poly_alloc(p->n_words);
  uint16_t i = 0;
  while(i < a->n_words) {
    a->vec[i] = p->vec[i];
    i++;
  }
  return a;
}

Poly* poly_create_from_coeffs(uint16_t t, const uint16_t *v, uint8_t n) {
  // TODO: v must be sorted?
  Poly *p = poly_alloc(t);
  uint16_t word;
  uint8_t pos;
  while(n--) {
    word = v[n] / W; // word to xor with. NOTE: word=0 <=> p->vec[p->t-1]
    pos = v[n] - (word * W); // bit position to xor
    p->vec[GET_VEC_WORD_INDEX(p->n_words, word)] ^= (0x1 << pos); // actual xor
  }
  return p;
}

int32_t poly_degree(const Poly *p) {
  uint16_t t = 0;
  
  while(p->vec[t] == 0 && t < (p->n_words - 1)) {
    t++;
  }
  return binary_degree(p->vec[t]) + (W * (p->n_words - t - 1));
}

// reallocs p->vec, if necessary, and updates p->t aswell
Poly *poly_refresh(Poly *p) {
  /* in how many words can p be stored? */
  uint16_t n_words = ceil((double)poly_degree(p) / (double) W);
  
  if(p->n_words > n_words) {
    #ifdef DEBUG
    fprintf(stderr, "INFO: reallocating p->vec\n");
    #endif
    p->vec = poly_vec_realloc(p->vec, p->n_words, n_words);
    p->n_words = n_words;
  }
  return p;
}

// returns a polynomial of degree at most m
Poly* poly_rand_uniform_poly(uint32_t m) {
  uint16_t t = ceil((double)m / (double)W);
  uint8_t s = W*t - m;
  
  Poly *p = poly_alloc(t);
  while(t--) {
    p->vec[t] = random_uniform_uint32();
  }
  p->vec[0] &= (0xffffffff >> s); // align last word
  return p;
}

// returns a polynomial of degree at most m
Poly* poly_rand_bernoulli_poly(uint32_t m, double tau) {
  uint16_t t = ceil((double)m / (double)W);
  uint8_t s = W*t - m;
  
  
  Poly *p = poly_alloc(t);
  while(t--)
    p->vec[t] = random_bernoulli_uint32(tau);
  p->vec[0] &= (0xffffffff >> s); // align last word
  return p;
}

// TODO: return in case of error. Use errno
uint8_t poly_get_bit(const Poly *a, uint32_t pos) {
  if(pos >= (a->n_words * W)) {
    #ifdef DEBUG
    fprintf(stderr, "ERROR: bit %u is out of range (max: %u - 1)\n", pos, (a->n_words * W));
    #endif
    return 2;
  }
  uint16_t word = BIT_WORD(pos); // in which word to find?
  uint8_t bit = pos % W; // bit position in the word
  return binary_get_bit(a->vec[GET_VEC_WORD_INDEX(a->n_words, word)], bit);
}

uint16_t poly_hamming_weight(const Poly *a) {
  uint16_t wt = 0;
  uint16_t t = a->n_words;
  while(t--)
    wt += binary_hamming_weight(a->vec[t]);
  return wt;
}

Poly *poly_get_r(const Poly *p) {
  Poly *r;
  uint16_t t = p->n_words;
  uint16_t i = 0;
  uint32_t p0 = p->vec[0] ^ (1 << binary_degree(p->vec[0]));
  if(p0 != 0) {
    r = poly_alloc(p->n_words);
    r->vec[0] = p0;
    for(i = 1; i < r->n_words; i++) {
      r->vec[i] = p->vec[i];
    }
  }
  else {
    t = 1;
    while(p->vec[t] == 0)
      t++;
    // now t is the first non zero word 
    r = poly_alloc(p->n_words - t);
    for(i = 0; i < r->n_words; i++) {
      r->vec[GET_VEC_WORD_INDEX(r->n_words, i)] = p->vec[GET_VEC_WORD_INDEX(p->n_words, i)];
    }
  }

  return r;
}

// set the poly n_words to nmemb
Poly *poly_realloc(Poly *a, uint16_t nmemb) {
  a->vec = poly_vec_realloc(a->vec, a->n_words, nmemb);
  a->n_words = nmemb;
  return a;
}

uint32_t *poly_vec_realloc(uint32_t *v, uint16_t t, uint16_t nmemb) {
  if(nmemb == t) {
    #ifdef DEBUG
    fprintf(stderr, "INFO: no realloc possible\n");
    #endif
    return v;
  }
  uint32_t *new_t;
  uint16_t i = 0;
  // remove the first words in the array <=> last words in the polynomial
  if(nmemb < t) {
    new_t = calloc(nmemb, sizeof(uint32_t));
    for(i = 0; i < nmemb; i++) {
      new_t[GET_VEC_WORD_INDEX(nmemb, i)] = v[GET_VEC_WORD_INDEX(t, i)];
    }
  }
  // insert (nmemb - t) words in the head of array <=> add more words to the polynomial
  else {
    new_t = calloc(nmemb, sizeof(uint32_t));
    for(i = 0; i < t; i++) {
      new_t[GET_VEC_WORD_INDEX(nmemb, i)] = v[GET_VEC_WORD_INDEX(t, i)];
    }
  }
  free(v);
  v = new_t;
  
  return new_t;
}

uint32_t** table_compute_mod_table(const Poly *f) {
  uint8_t k = 0;
  uint16_t i = 0;
  uint32_t **table; // 2-dimension array = table
  Poly *r = poly_get_r(f);
  // add an extra word to r
  r->vec = poly_vec_realloc(r->vec, r->n_words, r->n_words+1);
  r->n_words++;
  uint16_t r_t = r->n_words; // number of words in each u_k
  table = malloc(sizeof(uint32_t*) * W);
  
  uint32_t *u0 = calloc(r_t, sizeof(uint32_t));
  #ifdef DEBUG
  printf("INFO: computing mod table\n");
  #endif
  // copy r->vec to u0
  for(i = 0; i < r_t; i++) {
    //u0[i] = f->vec[i-1];
    u0[i] = r->vec[i];
  }
  table[0] = u0;

  k = 1;
  while(k < W) {
    table[k] = calloc(r_t, sizeof(uint32_t));
    binary_array_shift_left2(table[k - 1], r_t, table[k]);
    k++;
  }
  
  /*// print table  
  for(k = 0; k < W; k++) {
    printf("u[%u] = ", k);
    for(i = 0; i < r_t; i++) {
      printf("0x%.8x ", table[k][i]);
    }
    printf("\n");
  }*/
  poly_free(r);
  return table;
}

void table_free(uint32_t **t) {
  uint8_t i = 0;
  
  if(t != NULL) {
    
    for(i = 0; i < W; i++) {
      if(t[i] != NULL) {
        free(t[i]);
      }
    }
    free(t);
  }
}

// NOTE: the Poly a is changed, and the value returned is the same as a
// addapted from here and from prof: http://stackoverflow.com/questions/2773890/efficient-bitshifting-an-array-of-int
Poly* poly_shift_left(Poly *a, uint16_t i) {
  while(i--)
    binary_array_shift_left(a->vec, a->n_words);
  return a;
}

Poly* poly_shift_right(Poly *a, uint16_t i) {
  while(i--)
    binary_array_shift_right(a->vec, a->n_words);
  return a;
}

// returns c = a + b
// NOTE: a and c can have different lengths..
Poly* poly_add(const Poly *a, const Poly *b) {
  const Poly *bigger = a, *smaller = b;
  if(a->n_words < b->n_words) {
    bigger = b;
    smaller = a;
  }
  uint16_t diff = bigger->n_words - smaller->n_words;
  Poly *c = poly_alloc(bigger->n_words);
  uint16_t t = bigger->n_words;

  while(t--) {
    if(t >= diff) {
      // xor commons words of both polys
      c->vec[t] = bigger->vec[t] ^ smaller->vec[t - diff];
    }
    else {
      // no need to xor, just copy
      c->vec[t] = bigger->vec[t];
    }
  }
  return c;
}

Poly* poly_mult(const Poly *a, const Poly *b) {
  uint32_t a_m = poly_degree(a) + 1;
  uint32_t b_m = poly_degree(b) + 1;
  // use this a_t instead of a->n_words, because a->n_words may
  //not be up to date..
  uint16_t a_t = ceil((double)a_m / (double)W);
  //uint16_t b_t = ceil((double)b_m / (double)W);
  
  uint32_t m = (a_m > b_m) ? a_m : b_m;
  uint16_t t = ceil((double)m / (double)W);
  uint16_t c_t = t*2;
  Poly *c = poly_alloc(c_t);
  Poly *B = poly_clone(b);
  
  // B has one extra word than B
  B->vec = poly_vec_realloc(B->vec, B->n_words, B->n_words+1);
  B->n_words++;
  
  uint8_t k = 0;
  uint16_t j = 0;
  for(k = 0; k < W; k++) {
    for(j = 0; j < t; j++) {
      if(j < a_t) {
        uint8_t bit = binary_get_bit(a->vec[GET_VEC_WORD_INDEX(a->n_words, j)], k);
        if(bit == 1) {
          uint16_t j_aux = j;
          while(j_aux < c_t && (j_aux - j) < B->n_words) {
            c->vec[GET_VEC_WORD_INDEX(c->n_words,j_aux)] ^= 
                      B->vec[GET_VEC_WORD_INDEX(B->n_words, j_aux - j)];
            j_aux++;
          }
        }
      }
    }
    if(k != (W-1)) {
      poly_shift_left(B, 1);
    }
  }
  
  poly_free(B);
  //return poly_refresh(c);
  return c;
}

// TODO: use global table variable
Poly* poly_mod(const Poly *c, const Poly *f) {
  uint32_t c_m = poly_degree(c) + 1;
  // use this a_t instead of a->n_words, because a->n_words may
  //not be up to date..
  uint16_t c_t = ceil((double)c_m / (double)W);
  //uint8_t c_s = W * c_t - c_m;
  
  uint32_t m = poly_degree(f);
  uint16_t t = ceil((double)m / (double)W);
  
  uint8_t s = W*t - m;
  // TODO: dont do this in here..
  Poly *r = poly_get_r(f);
  uint16_t r_t = ceil((double)poly_degree(r) / (double)W) + 1;
  poly_free(r);
  uint32_t **table = table_compute_mod_table(f);
  Poly *a = poly_clone(c);
  
  uint16_t j, j_aux;
  uint8_t k;
  uint16_t i;
  for(i = c_m - 1; i >= m; i--) {
    
    uint8_t bit = poly_get_bit(a, i);
    if(bit == 1) {
      j = floor(((double)i - (double)m) / (double)W);
      k = (i - m) - (W * j);
      
      j_aux = j;
      // j_aux - j => word in table
      // j_aux => word in c
      while( (j_aux < c_t) && ((j_aux - j) < r_t)) {
         // C{j_aux} = C{j_aux} ^ u[k]
         
        uint32_t uk_k = GET_VEC_WORD_INDEX(r_t, j_aux - j);
        
        uint32_t uk = table[k][uk_k];
        
        uint16_t ci_i = GET_VEC_WORD_INDEX(c_t, j_aux);
        uint32_t ci = a->vec[ci_i];
        // C[j_aux] = C[j_aux] ^ u[k][j_aux - j]
        
        a->vec[ci_i] = ci ^ uk;
        
        j_aux++;
      }
    }
  }
  
  table_free(table);
  
  a->vec = poly_vec_realloc(a->vec, a->n_words, t);
  a->n_words = t;
  a->vec[0] &= (0xffffffff >> s); // align last word

  return a;
}

Poly *poly_fast_mod_irreduc(Poly *c, const Poly *f) {
  if(c->n_words != (f->n_words * 2)) {
    fprintf(stderr, "ERROR: poly c words\n");
    return NULL;
  }
  uint16_t i = c->n_words - 1;
  uint8_t s = 12;
  uint32_t T = 0;
  for( ; i >= f->n_words; i--) {
    T = c->vec[GET_VEC_WORD_INDEX(c->n_words, i)];
    c->vec[GET_VEC_WORD_INDEX(c->n_words, i - 17)] ^= ((T << 12) ^ (T << 13));
    c->vec[GET_VEC_WORD_INDEX(c->n_words, i - 16)] ^= ((T >> 20) ^ (T >> 19));
  }
  
  T = c->vec[GET_VEC_WORD_INDEX(c->n_words, 16)];
  c->vec[GET_VEC_WORD_INDEX(c->n_words, 0)] ^= ((T >> 20) ^ (T >> 19));
  c = poly_realloc(c, f->n_words);
  c->vec[0] &= 0xFFFFF;
  return c;
}

Poly* poly_mod_faster(Poly *c, const Poly *f, uint32_t ***tb) {
  uint32_t **table;
  if(tb == NULL) {
    #ifdef DEBUG
    printf("INFO: computing table in mod, and discarding its result\n");
    #endif
    table = table_compute_mod_table(f);
  }
  else {
    if(*tb == NULL) {
      #ifdef DEBUG
      printf("INFO: computing table in mod, but saving it for later\n");
      #endif
      *tb = table_compute_mod_table(f);
      table = *tb;
    }
    else {
      #ifdef DEBUG
      printf("INFO: using pre-computed table\n");
      #endif
      table = *tb;
    }
  }
  uint32_t c_m = poly_degree(c) + 1;
  // use this a_t instead of a->n_words, because a->n_words may
  //not be up to date..
  uint16_t c_t = ceil((double)c_m / (double)W);
  //uint8_t c_s = W * c_t - c_m;
  
  uint32_t m = poly_degree(f);
  uint16_t t = ceil((double)m / (double)W);
  
  uint8_t s = W*t - m;
  // TODO: dont do this in here..
  Poly *r = poly_get_r(f);
  uint16_t r_t = ceil((double)poly_degree(r) / (double)W) + 1;
  poly_free(r);
  //Poly *a = poly_clone(c);
  Poly *a = c;
  
  uint16_t j, j_aux;
  uint8_t k;
  uint16_t i;
  for(i = c_m - 1; i >= m; i--) {
    
    uint8_t bit = poly_get_bit(a, i);
    if(bit == 1) {
      j = floor(((double)i - (double)m) / (double)W);
      k = (i - m) - (W * j);
      
      j_aux = j;
      // j_aux - j => word in table
      // j_aux => word in c
      while( (j_aux < c_t) && ((j_aux - j) < r_t)) {
         // C{j_aux} = C{j_aux} ^ u[k]
         
        uint32_t uk_k = GET_VEC_WORD_INDEX(r_t, j_aux - j);
        
        uint32_t uk = table[k][uk_k];
        
        uint16_t ci_i = GET_VEC_WORD_INDEX(c_t, j_aux);
        uint32_t ci = a->vec[ci_i];
        // C[j_aux] = C[j_aux] ^ u[k][j_aux - j]
        
        a->vec[ci_i] = ci ^ uk;
        
        j_aux++;
      }
    }
  }
  if(tb == NULL) {
    table_free(table);
  }
  
  a->vec = poly_vec_realloc(a->vec, a->n_words, t);
  a->n_words = t;
  a->vec[0] &= (0xffffffff >> s); // align last word

  return a;
}

Poly *poly_xgcd(const Poly *a, const Poly *b, Poly **g, Poly **h) {
  fprintf(stderr, "ERROR: poly_xgcd not implemented yet!\n");
  return NULL;
}

// returns a*b mod f.
Poly *poly_mult_mod(const Poly *a, const Poly *b, const Poly *f) {
  Poly *m = poly_mult(a,b);
  Poly *mod = poly_mod(m, f);
  poly_free(m);
  return mod;
}

// returns a*b mod f. saves result in a
Poly *poly_mult_mod_faster(Poly *a, const Poly *b, const Poly *f, uint32_t ***table) {
  Poly *m = poly_mult(a,b);
  poly_free(a);
  m = poly_mod_faster(m, f, table);
  a = m;
  return m;
}

void poly_print_poly(const Poly *f) {
  if(!f) {
    fprintf(stderr, "ERROR poly NULL\n");
    return;
  }
  uint16_t i = 0;
  uint16_t t = f->n_words;
  unsigned char w[W+1];
  
  while(i < t) {
    printf("%s", binary_uint32_to_char(f->vec[i], w));
    i++;
  }
  printf("\n");
}
void poly_print_poly_hex(const Poly *f) {
  if(!f) {
    fprintf(stderr, "ERROR poly NULL\n");
    return;
  }
  uint16_t i = 0;
  uint16_t t = f->n_words;
  
  while(i < t) {
    printf("0x%.8x ", f->vec[i]);
    i++;
  }
  printf("\n");
}

/* Poly CRT */

PolyCRT* poly_crt_alloc(uint8_t m) {
  PolyCRT *p = malloc(sizeof(PolyCRT));
  if(!p) {
    fprintf(stderr, "ERROR poly_crt_alloc p\n");
    return NULL;
  }
  p->m = m;
  p->crt = calloc(m, sizeof(Poly*));
  if(!p->crt) {
    fprintf(stderr, "ERROR poly_crt_alloc p->crt\n");
    free(p);
    return NULL;
  }
  return p;
}

void poly_crt_free(PolyCRT *p) {
  if(p) {
    if(p->crt) {
      uint8_t m = p->m;
      while(m--) {
        poly_free(p->crt[m]);
      }
      free(p->crt);
    }
    free(p);
  }
}

// m: at most degree m
PolyCRT *poly_crt_rand_uniform(const PolyCRT *f) {
  // TODO: validation
  PolyCRT *a = poly_crt_alloc(f->m);
  if(!a) {
    return NULL;
  }
  uint8_t i = 0;
  for(i = 0; i < a->m; i++) {
    uint32_t deg = poly_degree(f->crt[i]);
    a->crt[i] = poly_rand_uniform_poly(deg);
  }
  return a;
}
// NOTE: deg must be equal to prod(f)
PolyCRT* poly_crt_rand_bernoulli(uint32_t deg, const PolyCRT *f, double tau) {
  Poly *ber = poly_rand_bernoulli_poly(deg, tau);
  PolyCRT *ber_crt = poly_to_crt(ber, f);
  poly_free(ber);
  return ber_crt;
}

// TODO: not finished
Poly *poly_crt_to_poly(const PolyCRT *a, const PolyCRT *f) {
  uint8_t i;
  Poly *N = NULL, *aux = f->crt[0];
  for(i = 1; i < f->m; i++) {
    if(i == 1) {
      N = poly_mult(aux, f->crt[i]);
      aux = N;
    }
    else {
      N = poly_mult(aux, f->crt[i]);
      poly_free(aux);
      aux = N;
    }
  }
  // TODO: extended euclidean algorithm and possibly division
  return N;
}

PolyCRT* poly_to_crt(const Poly *a, const PolyCRT *f) {
  // TODO: validate a and f
  uint8_t i = 0;
  PolyCRT *a_crt = poly_crt_alloc(f->m);
  for(i = 0; i < a_crt->m; i++) {
    a_crt->crt[i] = poly_mod(a, f->crt[i]);
  }
  return a_crt;
}

PolyCRT* poly_crt_add(const PolyCRT *a, const PolyCRT *b) {
  if(a->m != b->m) {
    fprintf(stderr, "ERROR: add CRT\n");
    return NULL;
  }
  PolyCRT *c = poly_crt_alloc(a->m);
  uint8_t i = 0;
  for(i = 0; i < c->m; i++) {
    c->crt[i] = poly_add(a->crt[i], b->crt[i]);
  }
  
  return c;
}

PolyCRT* poly_crt_mult(const PolyCRT *a, const PolyCRT *b, const PolyCRT *f) {
  if(a->m != b->m || a->m != f->m) {
    fprintf(stderr, "ERROR: mult CRT\n");
    return NULL;
  }
  PolyCRT *c = poly_crt_alloc(a->m);
  uint8_t i = 0;
  for(i = 0; i < c->m; i++) {
    Poly *mult = poly_mult(a->crt[i], b->crt[i]);
    c->crt[i] = poly_mod(mult, f->crt[i]);
    poly_free(mult);
  }
  
  return c;
}

void poly_crt_print_poly(const PolyCRT *f) {
  if(!f) {
    fprintf(stderr, "ERROR poly NULL\n");
    return;
  }
  int i = 0;
  for(i = 0; i < f->m; i++) {
    printf("f_%d = ", i); poly_print_poly(f->crt[i]);
  }
}
