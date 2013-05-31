#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "random.h"
#include "poly.h"
#include "binary.h"
#include "lapin.h"

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

void poly_print_poly(Poly *f) {
  
  uint8_t i = 0;
  uint8_t t = f->t;
   
  unsigned char w[W+1];
  
  while(i < t) {
    binary_uint32_to_char(f->vec[i], w);
    printf("%s | ", binary_uint32_to_char(f->vec[i], w));
    i++;
  }
  printf("\n");

}

int main() {
  srand((unsigned)time(NULL));

  unsigned char w[32];
  int i = 0;
  Poly *a = poly_alloc(128);
  Poly *b = poly_alloc(128);
  poly_set_coefs(a, F_PROD_REDUCIBLE[0]);
  poly_set_coefs(b, F_PROD_REDUCIBLE[1]);
  
  
  printf("a = ");poly_print_poly(a);
  printf("b = ");poly_print_poly(b);
  Poly *c = poly_mult(a, b);
  printf("a*b = ");poly_print_poly(c);
  printf("%d\n", c->t);
  
  poly_free(a);
  poly_free(b);
  poly_free(c);
  
  return 0;
}
