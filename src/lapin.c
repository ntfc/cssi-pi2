#include <stdio.h> // TODO: remove
#include <stdlib.h>
#include <math.h>
#include "lapin.h"
#include "random.h"
#include "binary.h"
#include "poly.h"

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


//PiMapping irreducible
//return poly
Poly* lapin_pimapping_irreduc(const Poly *f, const Challenge c, uint8_t sec_param) {
  int8_t k = 0;
  int16_t j = 0;
  uint8_t cj;
  Poly *p;
  //uint8_t words = CEILING(((double)sec_param/(double)8) / (double)sizeof(*c)); // ceil((sec_param/8) / 4)
  uint8_t words = ceil(((double)sec_param/(double)8) / (double)sizeof(*c)); // ceil((sec_param/8) / 4)
  
  // tmp copy of c
  Challenge tmpC = calloc(words, sizeof(*c)); // TODO: does it make sense?
  for(j = 0; j < words; j++)
    tmpC[j] = c[j];
  uint16_t coeffs[16];

  unsigned char w[32+1];
  uint8_t i = 0;
  
  for(j = 15; j >= 0; j--) {
    cj = tmpC[words - 1] & 0x1F;
    // here we use j instead of the (j-1) indicated in the paper because
    //we're in 0-index mode
    coeffs[j] = (16 * j) + cj;
    // now, bj is a poly coefficient
    // shift tmpC
    k = 5; // 0x1F is 5 bits
    while(k--) {
      binary_array_shift_right(tmpC, words);
    }    
  }
  free(tmpC);
  p = poly_create_poly_from_coeffs(f, coeffs, 16);
  
  if(poly_hamming_weight(p) > 16) {
    fprintf(stderr, "INFO wt(pi(c)) > 16!\n");
  }
  return p;
}

// TODO: rename PolyVec to PolyCRT maybe? And create alloc and free funtions
PolyVec* lapin_pimapping_reduc(const PolyVec *f, uint8_t m, const Challenge c, uint8_t sec_param) {
  uint8_t i = 0, j = 0;
  uint16_t to_pad = 0, new_m = 0;
  uint8_t c_t = ceil((double)sec_param / (double)W); // number of words in challenge
  PolyVec *v = malloc(sizeof(Poly) * m);
  
  for(i = 0; i < m; i++) {
    to_pad = poly_degree(f[i]) - sec_param;
    new_m = sec_param + to_pad;

    Poly *vi = poly_alloc(new_m);
    for(j = 0; j < c_t; j++) {
      vi->vec[GET_VEC_WORD_INDEX(vi->t, j)] = c[GET_VEC_WORD_INDEX(c_t, j)];
    }
    // add to_pad zeros to the right
    while(to_pad--) {
      binary_array_shift_left(vi->vec, vi->t);
    }
    // just in case
    vi->vec[0] &= (0xffffffff >> vi->s); // align last word
    v[i] = vi;
    
  }
  
  return v;
}

/*// TODO: implement pi-mappings
// c must be an array with 80 elems
void lapin_pimapping_reduc(const unsigned char *c) {
  uint8_t i = 0;
  uint8_t toPad = 0;
  for(i = 0; i < 5; i++) {
    toPad = binary_degree(F_PROD_REDUCIBLE[i], 4) - SEC_PARAM;
    printf("%d\n", toPad);
  }
}*/


//KeyGen
Key* key_generate(const Poly *f) {
  Key *key = malloc(sizeof(Key));
  if(key == NULL) {
    fprintf(stderr, "ERROR alloc key\n");
  }
  key->s1 = poly_rand_uniform_poly(f);
  //poly s'
  key->s2 = poly_rand_uniform_poly(f);
  
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

//generate c
//uint8_t n security parameter
Challenge lapin_reader_step1(uint8_t sec_param){
  Challenge c;
  c = challenge_generate(sec_param);
  return c;
}

//generate r, e
//calculate z
void lapin_tag_step2(const Key *key, const Poly *f, const Challenge c, Poly **z,
                     Poly **r, double tau, uint8_t sec_param, uint32_t ***table) {
  // TODO: validate table
  *r = poly_rand_uniform_poly(f);
  printf("r=");poly_print_poly(*r);
  Poly *e = poly_rand_bernoulli_poly(f, tau);
  printf("e=");poly_print_poly(e);
  
  Poly *pi = lapin_pimapping_irreduc(f, c, sec_param);
  printf("pi=");poly_print_poly(pi);
  printf("wt(pi)=%u\n", poly_hamming_weight(pi));
  
  // NOTE: this way all the memory can be free'd
  // r * (s * pi(c) + s') + e
  Poly *sTimesPi = poly_mod(poly_mult(key->s1, pi), f, table);
  
  //printf("s*pi mod f=");poly_print_poly(sTimesPi);

  Poly *sTimesPiPlusS2 = poly_add(sTimesPi, key->s2);
  //printf("s*pi + s'=");poly_print_poly(sTimesPiPlusS2);
  poly_free(sTimesPi);

  Poly *rTimesRest = poly_mod(poly_mult(*r, sTimesPiPlusS2), f, table);
  //printf("(r * (s*pi + s')) mod f="); poly_print_poly(rTimesRest);
  //rTimesRest = poly_mod(rTimesRest, f);
  poly_free(sTimesPiPlusS2);
  
  *z = poly_add(rTimesRest, e);
  //printf("r * (s*pi + s')+e="); poly_print_poly(*z);
  poly_free(rTimesRest);
  
  // free
  poly_free(pi);
  poly_free(e);
}

//verification
int lapin_reader_step3(const Key *key, const Poly *f, const Challenge c,
                       const Poly *z, const Poly *r, double tau1,
                       uint8_t sec_param, uint32_t ***table) {
  //TODO: IF R PERTENCE A R^*
  
  Poly *pi = lapin_pimapping_irreduc(f, c, sec_param);
  
  Poly *sTimesPi = poly_mod(poly_mult(key->s1, pi), f, table);
  Poly *sTimesPiPlusS2 = poly_add(sTimesPi, key->s2);
  poly_free(sTimesPi);
  
  Poly *rTimesRest = poly_mod(poly_mult(r, sTimesPiPlusS2), f, table);
  poly_free(sTimesPiPlusS2);
  
  Poly *e1 = poly_add(z, rTimesRest);
  
  poly_free(rTimesRest);
  //Poly *e1 = poly_mult(poly_add(z, r), poly_add(poly_mult(key->s1, pi), key->s2));

  int8_t ret = 0;

  if((double)poly_hamming_weight(e1) > (double)(poly_degree(f)*tau1)) {
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
  
  return ret;
}



