#include <stdio.h> // TODO: remove
#include <stdlib.h>
#include <math.h>
#include <string.h> // memcpy
#include "lapin.h"
#include "random.h"
#include "binary.h"
#include "poly.h"
#ifdef PERFORMANCE
 #include <time.h>
 #include "measure.h"
#endif

// n: security parameter in bits
// NOTE: Challenge must be free'd in the end
Challenge challenge_generate(uint8_t sec_param) {
  //uint8_t words = CEILING(((double)sec_param/(double)8) / (double)sizeof(uint32_t)); // convert SEC_PARAM to bytes
  uint8_t words = ceil(((double)sec_param/(double)8) / (double)sizeof(uint32_t)); // convert SEC_PARAM to bytes
  Challenge c = calloc(words, sizeof(uint32_t));
  uint8_t i = 0;
  
  uint8_t pad = (words * W) - sec_param;
  
  for(i = 0; i < words; i++) {
    c[i] = random_uniform_uint32();
  }
  c[0] &= (0xFFFFFFFF >> pad);
  return c;
}

void challenge_free(Challenge c) {
  if(c != NULL) {
    free(c);
  }
}

// If redec != 0, reduc = true
// keys are also generated 
Lapin* lapin_init(uint8_t reduc) {
  Lapin *l = malloc(sizeof(Lapin));

  if(!l) {
    fprintf(stderr, "ERROR lapin_init l\n");
    return NULL;
  }
  // TODO: create a function pointer in lapin structure to save the key gen function
  if(reduc) {
    // set both reducible and reducible_crt polys
    Lapin l_init = {.reduc = 1, .tau = (double)1/(double)6, .tau2 = 0.29,
              .sec_param = SEC_PARAM, .n = poly_degree(f_reducible),
              .f_normal = f_reducible, .f_crt = f_reducible_crt};
    memcpy(l, &l_init, sizeof(Lapin));
    l->key_crt = key_crt_generate(l->f_crt);
  }
  else {
    // set only  the irreducle poly
    Lapin l_init = {.reduc = 0, .tau = (double)1/(double)8, .tau2 = 0.27,
              .sec_param = SEC_PARAM, .n = poly_degree(f_irreducible),
              .f_normal = f_irreducible};
    memcpy(l, &l_init, sizeof(Lapin));
    l->key = key_generate(l->f_normal);
  }
  
  
  return l;
}

void lapin_end(Lapin *l) {
  if(l) {
    if(l->key) {
      key_free(l->key);
    }
    if(l->key_crt) {
      key_crt_free(l->key_crt);
    }
    free(l);
  }
}


//PiMapping irreducible
//return poly
Poly* lapin_pimapping_irreduc(const Lapin *lapin, const Challenge c) {
  int8_t k = 0;
  int16_t j = 0;
  uint8_t cj;

  Poly *p;

  uint8_t words = ceil(((double)lapin->sec_param/(double)8) / (double)sizeof(*c)); // ceil((sec_param/8) / 4)
  
  
  // TODO: improve this!
  // tmp copy of c
  Challenge tmpC = calloc(words, sizeof(*c)); // TODO: does it make sense?
  for(j = 0; j < words; j++)
    tmpC[j] = c[j];
  uint16_t coeffs[16];
  for(j = 15; j >= 0; j--) {
    cj = tmpC[words - 1] & 0x1F;
    // here we use j instead of the (j-1) indicated in the paper because
    //we're in 0-index mode
    coeffs[j] = (16 * j) + cj;
    // now, bj is a poly coefficient
    // shift tmpC
    k = 5; // 0x1F is 5 bits
    // TODO: implement binary_array_shift_left(a, t, i)
    while(k--) {
      binary_array_shift_right(tmpC, words);
    }    
  }
  free(tmpC);

  p = poly_create_from_coeffs(lapin->f_normal->n_words, coeffs, 16);
  #ifdef DEBUG
  if(poly_hamming_weight(p) < 16) {
    fprintf(stderr, "INFO wt(pi(c)) < 16!\n");
  }
  #endif
  return p;
}

PolyCRT* lapin_pimapping_reduc(const Lapin *lapin, const Challenge c) {
  // TODO: validate c
  uint8_t i = 0, j = 0;
  uint16_t to_pad = 0, new_m = 0;
  uint8_t c_t = ceil((double)lapin->sec_param / (double)W); // number of words in challenge
  PolyCRT *v = poly_crt_alloc(lapin->f_crt->m);
  if(!v) {
    fprintf(stderr, "ERROR: poly crt alloc\n");
    return NULL;
  }

  for(i = 0; i < lapin->f_crt->m; i++) {
    to_pad = poly_degree(lapin->f_crt->crt[i]) - lapin->sec_param;
    // TODO: new_m or new_m + 1??
    new_m = lapin->sec_param + to_pad;
    uint16_t new_t = ceil((double)new_m / (double)W);
    
    Poly *vi = poly_alloc(new_t);
    for(j = 0; j < c_t; j++) {
      vi->vec[GET_VEC_WORD_INDEX(vi->n_words, j)] = c[GET_VEC_WORD_INDEX(c_t, j)];
    }
    // add to_pad zeros to the right
    while(to_pad--) {
      binary_array_shift_left(vi->vec, vi->n_words);
    }
    // TODO: is this necessary??
    // just in case
    uint8_t new_s = W*new_t - new_m;
    vi->vec[0] &= (0xffffffff >> new_s); // align last word
    v->crt[i] = vi;
  }
  
  return v;
}

//KeyGen
Key* key_generate(const Poly *f) {
  if(!f) {
    fprintf(stderr, "ERROR: f is NULL\n");
    return NULL;
  }
  Key *key = malloc(sizeof(Key));
  if(key == NULL) {
    fprintf(stderr, "ERROR alloc key\n");
    return NULL;
  }
  uint32_t m = poly_degree(f);
  // poly s
  key->s1 = poly_rand_uniform_poly(m);
  //poly s'
  key->s2 = poly_rand_uniform_poly(m);
  
  return key;
}
KeyCRT* key_crt_generate(const PolyCRT *f) {
  if(!f) {
    fprintf(stderr, "ERROR: f is NULL\n");
    return NULL;
  }
  KeyCRT *key = malloc(sizeof(KeyCRT));
  if(key == NULL) {
    fprintf(stderr, "ERROR alloc key\n");
    return NULL;
  }
  key->s1 = poly_crt_rand_uniform(f);
  
  key->s2 = poly_crt_rand_uniform(f);
  
  return key;
}

void key_free(Key *k) {
  if(k != NULL) {
    if(k->s1 != NULL) {
      poly_free(k->s1);
    }
    if(k->s2 != NULL) {
      poly_free(k->s2);
    }
    free(k);
  }
}

void key_crt_free(KeyCRT *k) {
  if(k != NULL) {
    if(k->s1) {
      poly_crt_free(k->s1);
    }
    if(k->s2) {
      poly_crt_free(k->s2);
    }
    free(k);
  }
}

// TODO: is it possible to assign one of the poly in poly_mult, poly_add, poly_mod, etc?
int8_t lapin_tag(const Lapin *lapin, const Challenge c, void *r, void *z) {
  if(!lapin) {
    fprintf(stderr, "ERROR: lapin is NULL\n");
    return -1;
  }
  if(lapin->reduc && !lapin->key_crt) {
    fprintf(stderr, "ERROR: crt keys not set\n");
    return -1;
  }
  if(!lapin->reduc && !lapin->key) {
    fprintf(stderr, "ERROR: irreduc keys not set\n");
    return -1;
  }
  double tau1 = lapin->tau;
  
  #ifdef PERFORMANCE
  //struct timespec start, end, rand_start, rand_end;
  //TIME_START(start);
  clock_cycles cc_a, cc_b, cc_r_a, cc_r_b;  
  CLOCK_START(cc_a);
  #endif
  // TODO: validate r and z and c and keys
  if(lapin->reduc) {
    const KeyCRT *key = lapin->key_crt;
    const PolyCRT *fi = lapin->f_crt;

    // cast r and z
    PolyCRT **r_crt = (PolyCRT**)r;
    PolyCRT **z_crt = (PolyCRT**)z;
    
    #ifdef PERFORMANCE
    // exclude this from the measurement
    //TIME_START(rand_start);
    CLOCK_START(cc_r_a);
    #endif
    *r_crt = poly_crt_rand_uniform(fi);
    PolyCRT *e = poly_crt_rand_bernoulli(lapin->n, fi, tau1);
    #ifdef PERFORMANCE
    CLOCK_END(cc_r_b);
    //TIME_END(rand_end);
    #endif
    #ifdef DEBUG
    printf("e=");poly_crt_print_poly(e);
    #endif
    PolyCRT *pi = lapin_pimapping_reduc(lapin, c);

    PolyCRT* sTimesPi = poly_crt_mult(key->s1, pi, fi);

    PolyCRT* sTimesPiPlusS2 = poly_crt_add(sTimesPi, key->s2);
    poly_crt_free(sTimesPi);
    
    PolyCRT *rTimes = poly_crt_mult(*r_crt, sTimesPiPlusS2, fi);
    poly_crt_free(sTimesPiPlusS2);
    *z_crt = poly_crt_add(rTimes, e);
    poly_crt_free(rTimes);
    
    poly_crt_free(pi);
    poly_crt_free(e);
  }
  else {
    const Key *key = lapin->key;
    Poly **r_poly = (Poly**)r;
    Poly **z_poly = (Poly**)z;
    // TODO: this might be more efficient..
    const Poly *f = lapin->f_normal;
    #ifdef PERFORMANCE
    // exclude this from the measurement
    //TIME_START(rand_start);
    CLOCK_START(cc_r_a);
    #endif
    *r_poly = poly_rand_uniform_poly(lapin->n);

    Poly *e = poly_rand_bernoulli_poly(lapin->n, tau1);
    #ifdef PERFORMANCE
    CLOCK_END(cc_r_b);
    //TIME_END(rand_end);
    #endif
    Poly *pi = lapin_pimapping_irreduc(lapin, c);
    
    // NOTE: this way all the memory can be free'd
    // r * (s * pi(c) + s') + e
    Poly *sTimesPi = poly_mult_mod(key->s1, pi, f);
    /*Poly *sTimesPi = poly_mult(key->s1, pi);
    sTimesPi = poly_fast_mod_irreduc(sTimesPi, f);*/
    
    Poly *sTimesPiPlusS2 = poly_add(sTimesPi, key->s2);

    poly_free(sTimesPi);

    Poly *rTimesRest = poly_mult_mod(*r_poly, sTimesPiPlusS2, f);
    /*Poly *rTimesRest = poly_mult(*r_poly, sTimesPiPlusS2);
    rTimesRest = poly_fast_mod_irreduc(rTimesRest, f);*/
    poly_free(sTimesPiPlusS2);
    
    *z_poly = poly_add(rTimesRest, e);
    poly_free(rTimesRest);
    
    // free
    poly_free(pi);
    poly_free(e);
    
    // NOTE: this way all the memory can be free'd
    // r * (s * pi(c) + s') + e
    /**uint32_t **table = table_compute_mod_table(f);
    Poly *pi = lapin_pimapping_irreduc(lapin, c);
    
    // NOTE: this way all the memory can be free'd
    // z = r * (s * pi(c) + s') + e
    *z_poly = poly_mult(key->s1, pi);
    *z_poly = poly_mod_faster(*z_poly, f, &table);
    
    Poly *add1 = poly_add(*z_poly, key->s2);
    Poly *rTimesAdd1 = poly_mult(add1, *r_poly);
    poly_free(add1);
    *z_poly = poly_mult_mod_faster(*z_poly, rTimesAdd1, f, &table);
    poly_free(rTimesAdd1);
    
    add1 = poly_add(*z_poly, e);
    poly_free(*z_poly);
    *z_poly = add1;
    
    // free
    poly_free(pi);
    poly_free(e);
    table_free(table);*/

  }
  #ifdef PERFORMANCE
  CLOCK_END(cc_b);
  //TIME_END(end);
  
  /*uint32_t rand_duration = TIME_RESULT(rand_start, rand_end);
  uint32_t duration = TIME_RESULT(start, end) - rand_duration;
  fprintf(stdout, "Time elapsed in lapin_tag (reducible = %u)", lapin->reduc);
  fprintf(stdout, " = %u ms (%u ms in random)\n", duration, rand_duration);*/
  clock_cycles cc_rand = CLOCK_RESULT(cc_r_a, cc_r_b);
  clock_cycles cc_total = CLOCK_RESULT(cc_a, cc_b) - cc_rand;
  fprintf(stdout, "Time elapsed in lapin_tag (reducible = %u)", lapin->reduc);
  fprintf(stdout, " = %llu clock cycles (%llu in random)\n", cc_total, cc_rand);
  #endif
  return 1;
}

int8_t lapin_vrfy(const Lapin *lapin, const Challenge c, const void *r, const void *z) {
  if(!lapin) {
    fprintf(stderr, "ERROR: lapin is NULL\n");
    return 0;
  }
  // TODO: validate r and z and c and keys
  double tau2 = lapin->tau2;
  
  int8_t ret = 0;
  
  #ifdef PERFORMANCE
  //struct timespec start, end;
  //TIME_START(start);
  clock_cycles cc_a, cc_b;
  CLOCK_START(cc_a);
  #endif
  if(lapin->reduc) {
    const KeyCRT *key = lapin->key_crt;
    const PolyCRT *r_crt = (const PolyCRT *)r;
    const PolyCRT *z_crt = (const PolyCRT *)z;
    //TODO: IF R PERTENCE A R^*
    const PolyCRT *f = lapin->f_crt;
    
    PolyCRT *pi = lapin_pimapping_reduc(lapin, c);
    
    PolyCRT *sTimesPi = poly_crt_mult(key->s1, pi, f);
    
    PolyCRT *sTimesPiPlusS2 = poly_crt_add(sTimesPi, key->s2);
    poly_crt_free(sTimesPi);
    
    PolyCRT *rTimesRest = poly_crt_mult(r_crt, sTimesPiPlusS2, f);
    poly_crt_free(sTimesPiPlusS2);
    
    PolyCRT *e1 = poly_crt_add(z_crt, rTimesRest);
    poly_crt_free(rTimesRest);
    
    Poly *e1_poly = poly_crt_to_poly(e1, f);
    poly_crt_free(e1);
    if((double)poly_hamming_weight(e1_poly) > ((double)lapin->n * tau2)) {
      // reject
      ret = 0;
    }
    else {
      // accept
      ret = 1;
    }
    
    // free
    poly_free(e1_poly);
    poly_crt_free(pi);
  }
  else {
    const Key *key = lapin->key;
    const Poly *r_poly = (const Poly *)r;
    const Poly *z_poly = (const Poly *)z;
    //TODO: IF R PERTENCE A R^*
    const Poly *f = lapin->f_normal;
    
    Poly *pi = lapin_pimapping_irreduc(lapin, c);
    
    
    // NOTE: this way all the memory can be free'd
    // z - r * (s * pi + s')
    Poly *sTimesPi = poly_mult_mod(key->s1, pi, f);
    
    
    Poly *sTimesPiPlusS2 = poly_add(sTimesPi, key->s2);
    poly_free(sTimesPi);
    
    Poly *rTimesRest = poly_mult_mod(r_poly, sTimesPiPlusS2, f);
    poly_free(sTimesPiPlusS2);
    
    Poly *e1 = poly_add(z_poly, rTimesRest);
    
    poly_free(rTimesRest);
    
    if((double)poly_hamming_weight(e1) > ((double)lapin->n * tau2)) {
      // reject
      ret = 0;
    }
    else {
      // accept
      ret = 1;
    }
    
    // free
    poly_free(pi);
    poly_free(e1);
    // NOTE: this way all the memory can be free'd
    // z - r * (s * pi + s')
    /**uint32_t **table = table_compute_mod_table(f);
    Poly *e2 = poly_mult(key->s1, pi);
    Poly *add1 = poly_add(e2, key->s2);
    poly_free(e2);
    Poly *e1 = poly_add(r_poly, r_poly);
    
    e1 = poly_mult_mod_faster(e1, add1, f, &table);
    poly_free(add1);
    
    if((double)poly_hamming_weight(e1) > ((double)lapin->n * tau2)) {
      // reject
      ret = 0;
    }
    else {
      // accept
      ret = 1;
    }
    
    // free
    poly_free(pi);
    poly_free(e1);
    table_free(table);*/
  }
  #ifdef PERFORMANCE
  CLOCK_END(cc_b);
  //TIME_END(end);
  
  //uint32_t duration = TIME_RESULT(start, end);
  //fprintf(stdout, "Time elapsed in lapin_vrfy (reducible = %u) = %u ms\n",
  //                lapin->reduc, duration);
  clock_cycles cc_total = CLOCK_RESULT(cc_a, cc_b);
  fprintf(stdout, "Time elapsed in lapin_crfy (reducible = %u)", lapin->reduc);
  fprintf(stdout, " = %llu clock cycles\n", cc_total);
  #endif
  return ret;
}


/////////////////// 
// TODO: delete or refactor this
void challenge_print_challenge(const Challenge c) {
  uint8_t t = 0;
  unsigned char w[W+1];
  while(t < 3) {
    printf("%s", binary_uint32_to_char(c[t++], w));
  }
  printf("\n");
}
