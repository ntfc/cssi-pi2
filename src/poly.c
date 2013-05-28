#include <stdlib.h>
#include <stdio.h> // TODO: remove this
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
Poly poly_random_uniform_poly(const Poly f, uint8_t t) {
  // TODO: receive s as parameter?
  uint8_t s = (t*W) - binary_degree(f, t); // s = Wt - m
  printf("1st = 0x%x, t = %u, degree = %u, s = %u\n", f[0], t, binary_degree(f, t), s);
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
  printf("t = %u, degree = %u, s = %u\n", t, binary_degree(f, t), s);
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
  uint8_t k, j, actual_j;
  uint8_t i = 0;
  unsigned char w[32];
  size_t c_words = 2*t - 1; // number of words
 
  // allocate c
  *c = calloc(c_words, sizeof(uint32_t));
  Poly bAux = malloc(sizeof(uint32_t) * t);
  for(i = 0; i < t; i++) // copy b to bAux
    bAux[i] = b[i];
  for(k = 0; k < W; k++) {
    for(j = 0; j < t; j++) {
      // in our representation, (t - j) - 1 is the same as j in the right-to-left comb method
      actual_j = (t - j) - 1;
      uint8_t kthBit = binary_get_bit(a[actual_j], k+1);
      //printf("a[%d] = %s\n", actual_j, binary_uint_to_char(a[actual_j], w));
      if(kthBit == 1) {
        //printf("%d-th bit of A[%d] (actual j = %d) is 1\n", k, j, actual_j);
        // realloc bAux
        Poly temp = realloc(bAux, sizeof(uint32_t) * (t + j));
        if(temp != NULL) {
          bAux = temp;
        }
        // now: bAux = b with j words appended to the right
        //printf("bAux=");poly_print_poly(bAux, t + j);
        // difference of W-bit words between C and bAux
        uint8_t diffWords = c_words - (t + j);
        printf("diffWords = %d\n", diffWords);
        Poly cAux = poly_add(*c + diffWords, bAux, t+j);
        printf("cAux = ");poly_print_poly(cAux, t+j);
        for(i = diffWords; i < (t+j); i++) {
          printf("c[%d] = cAux[%d]\n", i, i-diffWords);
          //*c[i] = cAux[i - diffWords];
        }
        
      }
    }
  }
  free(bAux);
  /*// TODO: verifiy validity of c
  // traversing in reverse order, so a[0] = A[t], a[1] = A[t-1] .. a[t] = A[0]
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
      printf("verify A[%d][%d] || ", k, t-j);
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
        b = poly_shift_left(b, t);
      }
    }
    printf("\n");
  }*/
  
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

void poly_free(Poly p) {
  if(p != NULL)
    free(p);
}

void poly_print_poly(const Poly f, uint8_t t) {
  unsigned char w[32];
  uint8_t i = 0;
  while(i < t) {
    printf("%s", binary_uint_to_char(f[i], w));
    i++;
  }
  printf("\n");
}

uint16_t poly_hamming_weight(const Poly a, uint8_t t) {
  uint16_t wt = 0;
  while(t--)
    wt += binary_hamming_weight(a[t]);
  return wt;
}
