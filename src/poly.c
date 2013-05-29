#include <stdlib.h>

#include "random.h"
#include "binary.h"
#include "poly.h"

// TODO: confirma que p[0]%f[0] esta certo!

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

// t words
Poly poly_random_uniform_poly(const Poly f, uint8_t t) {
  // TODO: receive s as parameter?
  uint8_t s = (t*W) - binary_degree(f, t); // s = Wt - m
  Poly p = malloc(sizeof(uint32_t) * t);
  while(t--)
    p[t] = poly_random_uniform_32bit_word();
  p[0] &= (0xffffffff >> s); // align last word
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
Poly poly_random_bernoulli_poly(const Poly f, uint8_t t, double tau) {
  // TODO: receive s as parameter?
  uint8_t s = (t*W) - binary_degree(f, t); // s = Wt - m
  uint32_t *p = malloc(sizeof(uint32_t) * t);

  while(t--)
    p[t] = poly_random_bernoulli_32bit_word(tau);
  p[0] &= (0xffffffff >> s); // align last word
  return p;
}

// t: number of words
// returns c = a + b
Poly poly_add(const Poly a, const Poly b, uint8_t t) {
  Poly c = malloc(sizeof(uint32_t) * t);
  while(t--) {
    c[t] = a[t] ^ b[t];
  }
  return c;
}

// right-to-left comb method
// returns the number of words in C
// C is written to *c. c is allocated here
// TODO: should c be allocated here? becuase we have to return the number of words in C
uint8_t poly_mult(const Poly a, const Poly b, Poly *c, uint8_t t) {
  size_t c_words = 2*t - 1; // number of words in C
  uint8_t k, j, actual_j;
  uint8_t i = 0;

  Poly C = *c = poly_alloc(c_words);
  //C = poly_alloc(c_words);
  Poly B = poly_alloc(t);
  for(i = 0; i < t; i++)
    B[i] = b[i];
  for(k = 0; k < W; k++) {
    for(j = 0; j < t; j++) {
      // in our representation, (t - j) - 1 is the same as j in the right-to-left comb method
      actual_j = (t - j) - 1;
      uint8_t kthBit = binary_get_bit(a[actual_j], k+1);
      
      if(kthBit == 1) {
        // TODO: find a way to not use poly_alloc for newB, but instead use only the B
        Poly newB = poly_alloc(c_words);
        // put the B in newB
        for(i = 0; i < t; i++)
          newB[i + (c_words - t)] = B[i];
        // add j words to newB == shift j
        uint8_t toShift = j;
        while(toShift > 0) {
          poly_shift_left(newB, c_words);
          toShift--;
        }
        // add newB to C. save to tmp in order to free old C
        Poly tmp = poly_add(C, newB, c_words);
        poly_free(C);
        C = tmp;
        // free newB
        poly_free(newB);
      }
    }
    if(k != (W-1)) {
      B = poly_shift_left(B, t);
    }
  }
  // free B
  poly_free(B);
  // because poly_add creates a new poly, we need to re-set c's address
  *c = C;
  return c_words;
}

// NOTE: the Poly a is changed, and the value returned is the same as a
// addapted from here and from prof: http://stackoverflow.com/questions/2773890/efficient-bitshifting-an-array-of-int
Poly poly_shift_left(Poly a, uint8_t t) {
  uint8_t i = 0;
  for(i = 0; i < (t - 1); i++)
    a[i] = (a[i] << 1) | (a[i+1] >> (W - 1));
  a[i] <<= 1;
  return a;
}

Poly poly_shift_right(Poly a, uint8_t t) {
  uint8_t i = 0;
  for(i = 0; i < (t - 1); i++)
    a[i] = (a[i] >> 1) | (a[i+1] << (W - 1));
  a[i] >>= 1;
  return a;
}

uint16_t poly_hamming_weight(const Poly a, uint8_t t) {
  uint16_t wt = 0;
  while(t--)
    wt += binary_hamming_weight(a[t]);
  return wt;
}


Poly poly_alloc(uint8_t t) {
  Poly p = calloc(t, sizeof(uint32_t));
  if(p == NULL) // TODO: deal with errors
    return NULL;
  return p;
}

void poly_free(Poly p) {
  if(p != NULL)
    free(p);
}
