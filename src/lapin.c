#include <stdio.h> // TODO: remove
#include <stdlib.h>
#include <math.h>
#include <string.h> // memcpy
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

// If redec != 0, reduc = true
// keys are also generated 
Lapin* lapin_init(uint8_t reduc) {
  Lapin *l = malloc(sizeof(Lapin));

  if(!l) {
    fprintf(stderr, "ERROR lapin_init l\n");
    return NULL;
  }
  
  if(reduc) {
    // set both reducible and reducible_crt polys
    Lapin l_init = {.reduc = 1, .tau = (double)1/(double)6, .tau2 = 0.29,
              .sec_param = SEC_PARAM, .n = poly_degree(f_reducible),
              .f_normal = f_reducible, .f_crt = f_reducible_crt};
    memcpy(l, &l_init, sizeof(Lapin));
    
  }
  else {
    // set only  the irreducle poly
    Lapin l_init = {.reduc = 0, .tau = (double)1/(double)8, .tau2 = 0.27,
              .sec_param = SEC_PARAM, .n = poly_degree(f_irreducible),
              .f_normal = f_irreducible};
    memcpy(l, &l_init, sizeof(Lapin));
  }
  // TODO: generate keys in crt format
  l->key = key_generate(l->f_normal);
  
  return l;
}

void lapin_end(Lapin *l) {
  if(l) {
    if(l->key) {
      key_free(l->key);
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
  
  if(poly_hamming_weight(p) < 16) {
    fprintf(stderr, "INFO wt(pi(c)) < 16!\n");
  }
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
    new_m = lapin->sec_param + to_pad;

    // TODO: new_m or new_m + 1??
    Poly *vi = poly_alloc(new_m);
    for(j = 0; j < c_t; j++) {
      vi->vec[GET_VEC_WORD_INDEX(vi->n_words, j)] = c[GET_VEC_WORD_INDEX(c_t, j)];
    }
    // add to_pad zeros to the right
    while(to_pad--) {
      binary_array_shift_left(vi->vec, vi->n_words);
    }
    // TODO: is this necessary??
    // just in case
    //vi->vec[0] &= (0xffffffff >> vi->s); // align last word
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
  
  key->s1 = poly_rand_uniform_poly(m);
  //poly s'
  key->s2 = poly_rand_uniform_poly(m);
  
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

int8_t lapin_tag(const Lapin *lapin, const Challenge c, void *r, void *z) {
  if(!lapin) {
    fprintf(stderr, "ERROR: lapin is NULL\n");
    return -1;
  }
  if(!lapin->key) {
    fprintf(stderr, "ERROR: keys not set\n");
    return -1;
  }
  double tau1 = lapin->tau;
  uint16_t sec_param = lapin->sec_param;
  const Key *key = lapin->key;
  
  // TODO: validate r and z and c and keys
  if(lapin->reduc) {
    const PolyCRT *fi = lapin->f_crt;
    const Poly *f = lapin->f_normal;
    // cast r and z
    PolyCRT **r_crt = (PolyCRT**)r;
    PolyCRT **z_crt = (PolyCRT**)z;
    
    // TODO: improve this
    PolyCRT *s1 = poly_to_crt(key->s1, fi);
    PolyCRT *s2 = poly_to_crt(key->s2, fi);
    
    *r_crt = poly_crt_rand_uniform(fi);
    PolyCRT *e = poly_crt_rand_bernoulli(lapin->n, fi, tau1);

    PolyCRT *pi = lapin_pimapping_reduc(lapin, c);

    PolyCRT* sTimesPi = poly_crt_mult(s1, pi, fi);

    PolyCRT* sTimesPiPlusS2 = poly_crt_add(sTimesPi, s2);
    poly_crt_free(sTimesPi);
    
    PolyCRT *rTimes = poly_crt_mult(*r_crt, sTimesPiPlusS2, fi);
    
    *z_crt = poly_crt_add(rTimes, e);
    poly_crt_free(rTimes);
    
    poly_crt_free(pi);
    poly_crt_free(e);
    poly_crt_free(s1); poly_crt_free(s2);
  }
  else {
    Poly **r_poly = (Poly**)r;
    Poly **z_poly = (Poly**)z;
    // TODO: this might be more efficient..
    const Poly *f = lapin->f_normal;
    
    *r_poly = poly_rand_uniform_poly(lapin->n);

    Poly *e = poly_rand_bernoulli_poly(lapin->n, tau1);
    
    Poly *pi = lapin_pimapping_irreduc(lapin, c);
    
    // NOTE: this way all the memory can be free'd
    // r * (s * pi(c) + s') + e
    
    Poly *sTimesPi = poly_mult_mod(key->s1, pi, f);
    
    
    Poly *sTimesPiPlusS2 = poly_add(sTimesPi, key->s2);

    poly_free(sTimesPi);

    Poly *rTimesRest = poly_mult_mod(*r_poly, sTimesPiPlusS2, f);
    poly_free(sTimesPiPlusS2);
    
    *z_poly = poly_add(rTimesRest, e);
    poly_free(rTimesRest);
    
    // free
    poly_free(pi);
    poly_free(e);

  }
  
  return 1;
}

int8_t lapin_vrfy(const Lapin *lapin, const Challenge c, const void *r, const void *z) {
  if(!lapin) {
    fprintf(stderr, "ERROR: lapin is NULL\n");
    return 0;
  }
  // TODO: validate r and z and c and keys
  double tau2 = lapin->tau2;
  uint16_t sec_param = lapin->sec_param;
  const Key *key = lapin->key;
  int8_t ret = 0;
  
  
  if(lapin->reduc) {
    // TODO: vrfy
  }
  else {
    const Poly *r_poly = (const Poly *)r;
    const Poly *z_poly = (const Poly *)z;
    //TODO: IF R PERTENCE A R^*
    const Poly *f = lapin->f_normal;
    
    Poly *pi = lapin_pimapping_irreduc(lapin, c);
    
    Poly *sTimesPi = poly_mult_mod(key->s1, pi, f);
    
    
    Poly *sTimesPiPlusS2 = poly_add(sTimesPi, key->s2);
    poly_free(sTimesPi);
    
    Poly *rTimesRest = poly_mult_mod(r_poly, sTimesPiPlusS2, f);
    poly_free(sTimesPiPlusS2);
    
    Poly *e1 = poly_add(z_poly, rTimesRest);
    
    poly_free(rTimesRest);

    if((double)poly_hamming_weight(e1) > (double)(lapin->n * tau2)) {
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
  
  }
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
