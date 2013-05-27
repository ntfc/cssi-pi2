#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

// t: number of words
// returns c = a + b
Poly poly_add(Poly a, Poly b, uint8_t t) {
  Poly c = malloc(sizeof(uint32_t) * t);
  while(t--) {
    c[t] = a[t] ^ b[t];
  }
  return c;
}

// right-to-left comb method
// returns the number of words in C
// C is written to *c. c is allocated here
// TODO: should c be allocated here?
uint8_t poly_mult(Poly a, Poly b, Poly *c, uint8_t t) {
  uint8_t k, j;
  uint8_t i = 0;
  // TODO: verifiy validity of c
  // traversing in reverse order, so a[0] = A[t], a[1] = A[t-1] .. a[t] = A[0]
  size_t c_size = 2*t - 1; // number of words
  size_t b_size = t; // number of words in b
  *c = calloc(c_size, sizeof(uint32_t));
  /////// copy b to bAux
  unsigned char w[32];
  Poly bAux = calloc(b_size, sizeof(uint32_t));
  for(i = 0; i < b_size; i++)
    bAux[i] = b[i];
  /////// end of copy b to bAux
  //unsigned char w[32];
  for(k = 0; k < W; k++) {
    //// j goes from t to 1, so we must use j-1 inside this for loop
    //for(j = t; j > 0; j--) {
    for(j = 0; j < t; j++) {
      //printf("(k,j) = (%2d, %2d)\n", k, j-1);
      uint8_t kThBit = binary_get_bit(a[t-j], k);
      if(kThBit == 1) {
        printf("add B to C{%d}\n", j-1);
        // add j W-bit words to the right of C
        // j W-bit words = sizeof(uint32_t) * j
        // TODO: is this right?
        b_size += (j-1);
        //bAux = realloc(bAux, sizeof(uint32_t) * b_size);
        bAux = realloc(bAux, sizeof(uint32_t) * (j-1));
        printf("bAux = ");
        for(i = 0; i < b_size; i++) {
          printf("%s", binary_uint_to_char(b[i], w));
        }
        printf("\n");
        // TODO: use temporary pointer here, in case realloc fails
        
        
        // difference of W-bit words between C and bAux
        
        // cAux is the adition between C and bAux
        
        // concat disaligned C with calculated cAux
      }
      if(k != (W-1)) {
        //printf("B = B.x\n");
        b = poly_shift_left(b);
      }
    }
  }
  
  return c_size;
}

Poly poly_shift_left(Poly a) {
  // TODO implement shift left to polynomial
  return a;
}

void poly_free(Poly p) {
  if(p != NULL)
    free(p);
}
