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

void poly_print_poly(const Poly f, uint8_t t) {
  unsigned char w[32];
  uint8_t i = 0;
  while(i < t) {
    printf("%s", binary_uint_to_char(f[i], w));
    i++;
  }
  printf("\n");
}

int main() {
  srand((unsigned)time(NULL));
  //printf("U(0,1) = %.8f\n", uniform_rand());
  //printf("Ber = %d\n", bernoulli(1/8));
  //printf("wt(0x%x) = %d\n",f_red_bin[0][3], hamming_weight(f_red_bin[0][3]));
  //printf("deg(x^532 + x + 1) = %d\n", degree(f_irr_bin, 17));
  //printf("%p\n", gen_bernoulli_rand_poly(f_irr_bin, 17, (double)1/(double)8));
  //memcpy(b, gen_bernoulli_rand_poly(f_irr_bin, 17, (double)1/(double)8), W);
  //Poly c = poly_random_uniform_poly(f_irr_bin, 17);
  unsigned char w[32];
  int i = 0;
  // x^167 + x^166 + x^165 + x^164 + x^132 + x^129 + x^103 + x^97 + x^68 + x^65 + x^37 + x^36 + x^33 + x^4 + x
  //uint32_t a[17] = { 0,0,0,0,0,0,0x0, 0x0, 0x0, 0x0, 0x0, 0xF0, 0x12, 0x82, 0x12, 0x32, 0x12};
  uint32_t a[2] = { 0x0, 0x82};
  /*printf("a = \n");
  for(i = 0; i < 17; i++)
    printf("%s", (binary_uint_to_char(a[i], w)));
  printf("\nb = \n");*/
  //uint32_t b[17] = {0x8302, 0x80a008, 0x40000, 0x80120802, 0x10002, 0x8080000, 0x200810, 
  //                  0x4110, 0x30240008, 0x8030, 0x480000, 0x80040000, 0x4b00083, 0x4280200,
  //                  0xa8041002, 0x680050, 0x40090 };
  uint32_t b[2] = {0x02, 0x80120802};
  /*for(i = 0; i < 17; i++)
    printf("%s", (binary_uint_to_char(b[i], w)));
  printf("\n");
  Poly d = poly_add(a, b, 17);
  printf("\na + b = \n");
  for(i = 0; i < 17; i++)
    printf("%s", (binary_uint_to_char(d[i], w)));
  printf("\n");*/
  printf(" a = "); poly_print_poly(a, 2);
  printf(" b = "); poly_print_poly(b, 2);
  Poly e;
  uint8_t new_t = poly_mult(a, b, &e, 2);
  printf("ab = "); poly_print_poly(e, 3);
  /*
   * printf("\na*b = ");
  for(i = 0; i < new_t; i++) {
    printf("%s", binary_uint_to_char(e[i], w));
  }
  printf("\n");
  */
  //printf("sizeof(f_irr) = %u\n", sizeof(f_irr_bin)/sizeof(f_irr_bin[0]));

  /*Poly f = poly_random_uniform_poly(f_red_bin[1], 5);
  printf("%p\n", &f);
  teste(&f, 5);
  printf("%p\n", &f);
  f[5] = 0xF;
  printf("f[5] = %s\n", binary_uint_to_char(f[5], w));
  
  printf("%p\n", &f);
  teste(&f, 6);
  printf("%p\n", &f);
  f[6] = 0xFF;
  printf("f[6] = %s\n", binary_uint_to_char(f[6], w));
  
  printf("%p\n", &f);
  teste(&f, 7);
  printf("%p\n", &f);
  f[7] = 0xFFF;
  printf("f[7] = %s\n", binary_uint_to_char(f[7], w));
  
  printf("%p\n", &f);
  teste(&f, 8);
  printf("%p\n", &f);
  f[8] = 0xFFFF;
  printf("f[8] = %s\n", binary_uint_to_char(f[8], w));*/
  return 0;
}