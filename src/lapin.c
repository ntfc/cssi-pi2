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
    union mod_poly f;
    f.crt = f_reducible_crt;
    f.normal = f_reducible;
    
    Lapin l_init = {.reduc = 1, .tau = (double)1/(double)6, .tau2 = 0.29,
              .sec_param = SEC_PARAM, .n = f_reducible->m, .f = f};
    memcpy(l, &l_init, sizeof(Lapin));
  }
  else {
    // set only  the irreducle poly
    union mod_poly f;
    f.normal = f_irreducible;
    
    Lapin l_init = {.reduc = 0, .tau = (double)1/(double)8, .tau2 = 0.27,
              .sec_param = SEC_PARAM, .n = f_irreducible->m, .f = f};
    memcpy(l, &l_init, sizeof(Lapin));
  }
  l->key = key_generate(l->f.normal);
  
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
Poly* lapin_pimapping_irreduc(const Poly *f, const Challenge c, uint8_t sec_param) {
  int8_t k = 0;
  int16_t j = 0;
  uint8_t cj;
  Poly *p;

  uint8_t words = ceil(((double)sec_param/(double)8) / (double)sizeof(*c)); // ceil((sec_param/8) / 4)
  
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

PolyCRT* lapin_pimapping_reduc(const PolyCRT *f, const Challenge c, uint8_t sec_param) {
  // TODO: validate c
  uint8_t i = 0, j = 0;
  uint16_t to_pad = 0, new_m = 0;
  uint8_t c_t = ceil((double)sec_param / (double)W); // number of words in challenge
  PolyCRT *v = poly_crt_alloc(f->m);
  
  for(i = 0; i < f->m; i++) {
    to_pad = poly_degree(f->crt[i]) - sec_param;
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
    v->crt[i] = vi;
    
  }
  
  return v;
}

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

int8_t lapin_tag(const Lapin *lapin, const Challenge c, Poly **r, Poly **z) {
  if(!lapin) {
    fprintf(stderr, "ERROR: lapin is NULL\n");
    return 0;
  }
  double tau1 = lapin->tau;
  uint16_t sec_param = lapin->sec_param;
  const Key *key = lapin->key;
  
  // TODO: validate r and z and c and keys
  if(lapin->reduc) {
    // TODO: tag
  }
  else {
    // TODO: this might be more efficient..
    Poly *f = lapin->f.normal;
    // TODO: use a global variable for table!
    uint32_t **table = poly_compute_mod_table(f);
    
    *r = poly_rand_uniform_poly(f);

    Poly *e = poly_rand_bernoulli_poly(f, tau1);
    
    Poly *pi = lapin_pimapping_irreduc(f, c, sec_param);
    
    // NOTE: this way all the memory can be free'd
    // r * (s * pi(c) + s') + e
    
    Poly *sTimesPi = poly_mult(key->s1, pi);
    Poly *sTimesPiMod = poly_mod(sTimesPi, f, &table);
    poly_free(sTimesPi);
    
    Poly *sTimesPiPlusS2 = poly_add(sTimesPiMod, key->s2);

    poly_free(sTimesPiMod);

    Poly *rTimesRest = poly_mult(*r, sTimesPiPlusS2);
    poly_free(sTimesPiPlusS2);
    Poly *rTimesRestMod = poly_mod(rTimesRest, f, &table);
    poly_free(rTimesRest);
    
    *z = poly_add(rTimesRestMod, e);
    poly_free(rTimesRestMod);
    
    // free
    poly_free(pi);
    poly_free(e);
    
    poly_free_table(table);
  }
  
  return 1;
}

int8_t lapin_vrfy(const Lapin *lapin, const Challenge c, const Poly *r, const Poly *z) {
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
    //TODO: IF R PERTENCE A R^*
    Poly *f = lapin->f.normal;
    uint32_t **table = poly_compute_mod_table(f);
    
    Poly *pi = lapin_pimapping_irreduc(f, c, sec_param);
    
    Poly *sTimesPi = poly_mult(key->s1, pi);
    Poly *sTimesPiMod = poly_mod(sTimesPi, f, &table);
    poly_free(sTimesPi);
    Poly *sTimesPiPlusS2 = poly_add(sTimesPiMod, key->s2);
    poly_free(sTimesPiMod);
    
    Poly *rTimesRest = poly_mult(r, sTimesPiPlusS2);
    Poly *rTimesRestMod = poly_mod(rTimesRest, f, &table);
    poly_free(rTimesRest);
    poly_free(sTimesPiPlusS2);
    
    Poly *e1 = poly_add(z, rTimesRestMod);
    
    poly_free(rTimesRestMod);



    if((double)poly_hamming_weight(e1) > (double)(poly_degree(f)*tau2)) {
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
    
    poly_free_table(table);
  
  }
  return ret;
}



