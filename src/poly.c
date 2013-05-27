#include <stdlib.h>

#include "random.h"
#include "binary.h"
#include "poly.h"

// generate a random number between
uint32_t poly_random_uniform_32bit_word() {
  unsigned char word[W] = {0};
  int i = 0;
  for(; i < W; i++) {
    // convert (int)0 to '0' or (int)1 to '1'
    word[i] = '0'+random_uniform_range(0, 1);
  }
  return binary_char_to_uint(word);
}

// TODO: generate a poly smaller than f
// t words
Poly poly_random_uniform_poly(uint32_t *f, uint8_t t) {
  // TODO: use malloc?
  Poly p = malloc(sizeof(uint32_t) * t);
  
  while(t--)
    p[t] = poly_random_uniform_32bit_word();
  p[0] = p[0] % f[0];
  return p;
}

// generate a random word, according to bernoulli distribution
uint32_t poly_random_bernoulli_32bit_word(double tau) {
  unsigned char word[W] = {0};
  int i = 0;
  for(; i < W; i++) {
    // convert (int)0 to '0' or (int)1 to '1'
    word[i] = '0'+random_bernoulli(tau);
  }
  return binary_char_to_uint(word);
}

// f: polynomial of degree m
// t: number of words in f
Poly poly_random_bernoulli_poly(uint32_t *f, uint8_t t, double tau) {
  // TODO: use malloc?
  uint32_t *p = malloc(sizeof(uint32_t) * t);
  
  while(t--)
    p[t] = poly_random_bernoulli_32bit_word(tau);
  p[0] = p[0] % f[0];
  return p;
}

void poly_free(Poly p) {
  if(p != NULL)
    free(p);
}
