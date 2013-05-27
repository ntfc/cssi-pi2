#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "random.h"
#include "poly.h"
#include "binary.h"

// http://stackoverflow.com/a/6556588/1975046
#define CEILING_POS(X) ((X-(int)(X)) > 0 ? (int)(X+1) : (int)(X))
#define CEILING_NEG(X) ((X-(int)(X)) < 0 ? (int)(X-1) : (int)(X))
#define CEILING(X) ( ((X) > 0) ? CEILING_POS(X) : CEILING_NEG(X) )

// TODO: check http://crypto.stackexchange.com/questions/8388/polynomial-multiplication-and-division-in-2128
// TODO: define the default word size
// NOTE: use the urandom to create the keys and random polynomials => random reducible poly == genreate 4 random uint32_t, 5 times

// polinomios sao arrays de uint32_t. uint32_t esta em stdint.h

// representacao mais agradavel
uint16_t f_irr_int[3] = {532, 1, 0};
// representacao binaria, mais eficiente
uint32_t f_irr_bin[17] = { 0x00100000, 0x00000000, 0x00000000, 0x00000000,
                           0x00000000, 0x00000000, 0x00000000, 0x00000000,
                           0x00000000, 0x00000000, 0x00000000, 0x00000000,
                           0x00000000, 0x00000000, 0x00000000, 0x00000000,
                           0x00000003
                         }; // x^532 + x + 1
// representacao mais agradavel
uint8_t f_red_int[5][5] = { {127, 8, 7, 3, 0},
                            {126, 9, 6, 5, 0},
                            {125, 9, 7, 4, 0},
                            {122, 7, 4, 3, 0},
                            {121, 8, 5, 1, 0}
                          };
// representacao binaria, mais eficiente
uint32_t f_red_bin[5][4] = { { 0x80000000, 0x00000000, 0x00000000, 0x00000189 }, // x^127+x^8+x^7+x^3+1
                             { 0x40000000, 0x00000000, 0x00000000, 0x00000261 }, // x^126+x^9+x^6+x^5+1
                             { 0x20000000, 0x00000000, 0x00000000, 0x00000291 }, // x^125+x^6+x^7+x^4+1
                             { 0x04000000, 0x00000000, 0x00000000, 0x00000099 }, // x^122+x^7+x^4+x^3+1
                             { 0x02000000, 0x00000000, 0x00000000, 0x00000123 }  // x^121+x^8+x^5+x+1
                           };

// print 32 chars from a string
void test_print_char_word(const unsigned char *c) {
  const unsigned char *s = c;
  while(*c && c-s < W) {
    printf("%c", *c);
    c++;
  }
  printf("\n");
}

//uint32_t* random_ring_element(uint32_t* f, uint32_t m) {
  
//}

int main() {
  srand((unsigned)time(NULL));
  //printf("U(0,1) = %.8f\n", uniform_rand());
  //printf("Ber = %d\n", bernoulli(1/8));
  //printf("wt(0x%x) = %d\n",f_red_bin[0][3], hamming_weight(f_red_bin[0][3]));
  //printf("deg(x^532 + x + 1) = %d\n", degree(f_irr_bin, 17));
  //printf("%p\n", gen_bernoulli_rand_poly(f_irr_bin, 17, (double)1/(double)8));
  //memcpy(b, gen_bernoulli_rand_poly(f_irr_bin, 17, (double)1/(double)8), W);
  //Poly c = poly_random_uniform_poly(f_irr_bin, 17);
  Poly c = poly_random_bernoulli_poly(f_irr_bin, 17, (double)1/(double)8);
  unsigned char w[32];
  int i = 0;
  for(; i < 17; i++) {
    printf("%s", binary_uint_to_char(f_irr_bin[i], w));
  }
  printf("\n");
  for(i = 0; i < 17; i++) {
    printf("%s", binary_uint_to_char(c[i], w));
  }
  printf("\n");
  poly_free(c);
  return 0;
}
