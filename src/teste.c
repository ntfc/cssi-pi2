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
  
  uint16_t i = 0;
  uint16_t t = f->t;
  unsigned char w[W+1];
  
  while(i < t) {
    printf("%s", binary_uint32_to_char(f->vec[i], w));
    i++;
  }
  printf("\n");

}

void teste2() {

  unsigned char w[32];
  int i = 0;
  Poly *a = poly_alloc(128, 4);
  Poly *b = poly_alloc(128, 4);
  uint32_t A[4] = {0x0, 0x0, 0x0, 0x324};
  uint32_t B[4] = {0x0, 0xF, 0xC, 0xCF12};
  poly_set_coeffs_from_uint32(a, A);
  poly_set_coeffs_from_uint32(b, B);
  
  
  printf("a = ");poly_print_poly(a);
  printf("b = ");poly_print_poly(b);
  Poly *c = poly_mult(a, b);
  printf("a*b = ");poly_print_poly(c);
  
  poly_free(a);
  poly_free(b);
  poly_free(c);
  
  // generate challenge
  Challenge ch = challenge_generate(SEC_PARAM);
  for(i = 0; i < 3; i++) {
    printf("%s", binary_uint32_to_char(ch[i], w));
  }
  printf("\n");
  uint32_t sh[2] = {0x0F, 0x00000000};
  printf("0x%.8x 0x%.8x\n", sh[0], sh[1]);
  binary_array_shift_right(sh, 2);
  printf("0x%.8x 0x%.8x\n", sh[0], sh[1]);
  
  Poly *f = poly_alloc(532, 17);
  poly_set_coeffs_from_uint32(f, F_IRREDUCIBLE);
  Poly *pi = lapin_pimapping_irreduc(f, ch, SEC_PARAM);
  poly_print_poly(pi);
  free(ch);
  
  uint16_t coeffs1[] = {128, 54, 1, 0, 3};
  uint16_t coeffs2[] = {500, 148, 128, 32, 64, 56, 87, 74, 52, 10, 111, 12, 15, 25, 0};
  Poly *a1 = poly_create_poly_from_coeffs(f, coeffs1, 5);
  Poly *a2 = poly_create_poly_from_coeffs(f, coeffs2, 15);
  Poly *c1 = poly_mult(a1, a2);
  poly_print_poly(a1); poly_print_poly(a2);
  printf("Before mod=");poly_print_poly(c1);
  c1 = poly_mod(c1, f);
  printf(" After mod=");poly_print_poly(c1);
}

int main() {
  uint16_t i = 0;
  srand((unsigned)time(NULL));
  unsigned char w[32];
  
  Poly *f = poly_alloc(532, 17);
  poly_set_coeffs_from_uint32(f, F_IRREDUCIBLE);
  printf("f=");poly_print_poly(f);
  
  uint32_t ch[3] =  {0xb9fe, 0x9d532bf9, 0x1ffa5b10};
  printf("ch=");
  for(i = 0; i < 3; i++) {
    printf("%s", binary_uint32_to_char(ch[i], w));
  }
  printf("\n");
  
  
  Poly *pi = lapin_pimapping_irreduc(f, ch, SEC_PARAM);
  printf("pi=");poly_print_poly(pi);
  /*Challenge c = challenge_generate(SEC_PARAM);
  printf("c=");
  for(i = 0; i < 3; i++) {
    printf("%s", binary_uint32_to_char(c[i], w));
  }
  printf("\n");
  Poly *r, *z;
  Key *key = key_generate(f);
  printf("key1=");poly_print_poly(key->s);
  printf("key2=");poly_print_poly(key->s1);
  lapin_tag_step2(key, f, c, &z, &r, (double)1/(double)8, SEC_PARAM);
  printf("r=");poly_print_poly(r);
  printf("z=");poly_print_poly(z);
  printf("Vrfy = %d\n", lapin_reader_step3(key, f, c, z, r, (double)0.27, SEC_PARAM));
  poly_free(f);
  poly_free(z);
  poly_free(r);
  challenge_free(c);
  key_free(key);*/
  return 0;
}
