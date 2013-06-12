#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

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

  /*unsigned char w[32];
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
  printf(" After mod=");poly_print_poly(c1);*/
}

void test_get_bit(const Poly *f) {
  printf("bit 600 = %u\n", poly_get_bit(f, 600));
  printf("bit 544 = %u\n", poly_get_bit(f, 544));
  printf("bit 532 = %u\n", poly_get_bit(f, 532));
  printf("bit 500 = %u\n", poly_get_bit(f, 500));
  printf("bit 2 = %u\n", poly_get_bit(f, 2));
  printf("bit 1 = %u\n", poly_get_bit(f, 1));
  printf("bit 0 = %u\n", poly_get_bit(f, 0));
}

void test_mult(const Poly *f) {
  Poly *a = poly_rand_uniform_poly(f);
  Poly *b = poly_rand_uniform_poly(f);
  
  Poly *c = poly_mult(a, b);
  printf("a=");poly_print_poly(a);
  printf("b=");poly_print_poly(b);
  printf("a*b=");poly_print_poly(c);

  poly_free(a);
  poly_free(b);
  poly_free(c);
}

void test_mod(const Poly *f) {
/*  // x^233 + x^74 + 1
  uint32_t new_f_coefs[] = {0x200, 0x0, 0x0, 0x0, 0x0, 0x400, 0x0, 0x1};
  Poly *new_f = poly_alloc(233, 8);
  poly_set_coeffs_from_uint32(new_f, new_f_coefs);*/
  Poly *a = poly_rand_uniform_poly(f);
  Poly *b = poly_rand_uniform_poly(f);
  printf("a=");poly_print_poly(a);
  printf("b=");poly_print_poly(b);
  
  Poly *c = poly_mult(a, b);
  printf("c=");poly_print_poly(c);
  uint32_t **table = NULL;
  Poly *d = poly_mod(c, f, table);
  printf("c mod=");poly_print_poly(d);
  //poly_free(a);
  //poly_free(b);
  //poly_free(c);
}

// f must be the irreducible polynomial
uint32_t** compute_table(const Poly *r) {
  uint8_t k = 0;
  uint16_t i = 0;
  uint32_t **table; // 2-dimension array = table
  uint16_t t = r->t + 1; // number of word in u_k
  table = malloc(sizeof(uint32_t*) * W);
  
  uint32_t *u0 = calloc(t, sizeof(uint32_t));
  // copy r->vec to u0. u0[0] = 0x0, u0[1] = f->vec[0], etc
  for(i = 1; i < t; i++) {
    u0[1] = r->vec[i-1];
  }
  table[0] = u0;
  
  k = 1;
  while(k < W) {
    printf("Computing x^%u . r(x)\n", k);
    table[k] = calloc(t, sizeof(uint32_t));
    binary_array_shift_left2(table[k - 1], t, table[k]);
    k++;
  }
  return table;
}

void test_mod2(const Poly *f) {
  Poly *r = poly_get_r(f);
  printf("r=");poly_print_poly(r);
  
  // compute table. save it for future use
  //uint32_t **table = compute_table(r);
  uint32_t **table = poly_compute_mod_table(r);
  uint16_t i = 0;
  
  
  Poly *a = poly_rand_uniform_poly(f);
  Poly *b = poly_rand_uniform_poly(f);
  //printf("a=");poly_print_poly(a);
  //printf("b=");poly_print_poly(b);
  
  Poly *c = poly_mult(a, b);
  printf("c=");poly_print_poly(c);
  
  Poly *C = poly_alloc(c->m , c->t);
  
  // copy c->vec to C->vec
  for(i = 0; i < c->t; i++) {
    C->vec[i] = c->vec[i];
  }
  printf("C=");poly_print_poly(C);
  
  uint16_t j;
  uint8_t k;
  uint16_t m = f->m;
  uint16_t j_aux;
  for(i = (2 * f->m) - 2; i >= f->m; i--) {
    uint8_t ci = poly_get_bit(C, i);
    if(ci == 1) {
      j = floor(((double)i - (double)m) / (double)W);
      k = (i - m) - (W * j);
      //printf("j = %u, i = %u\n", j, k);
      j_aux = j;
      while( (j_aux < c->t) && ((j_aux - j) <= r->t)) {
        // C{j_aux} = C{j_aux} ^ u[k]
        C->vec[GET_VEC_WORD_INDEX(C->t, j_aux)] ^= table[k][GET_VEC_WORD_INDEX(r->t + 1, j_aux - j)];
        j_aux++;
      }
    }
  }
  printf("C almost mod=");poly_print_poly(C);
  Poly *cMod = poly_alloc(a->m, a->t);
  for(i = 0; i < cMod->t; i++) {
    cMod->vec[(cMod->t - 1) - i] = C->vec[(C->t - 1) - i];
  }
  cMod->vec[0] &= (0xffffffff >> cMod->s); // align last word
  printf("c mod f=");poly_print_poly(cMod);
  poly_free(cMod);
  
  
  poly_free_table(table, W);
  poly_free(r);
  poly_free(a);
  poly_free(b);
  poly_free(c);
  poly_free(C);
}

void test_lapin(const Poly *f) {
  uint16_t i = 0;
  unsigned char w[32];
  Challenge c = challenge_generate(SEC_PARAM);
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
  //printf("r=");poly_print_poly(r);
  //printf("z=");poly_print_poly(z);
  //printf("Vrfy = %d\n", lapin_reader_step3(key, f, c, z, r, (double)0.27, SEC_PARAM));
  
  //poly_free(z);
  //poly_free(r);
  challenge_free(c);
  key_free(key);
}

int main() {
  srand((unsigned)time(NULL));
  
  Poly *f = poly_alloc(532, 17);
  poly_set_coeffs_from_uint32(f, F_IRREDUCIBLE);
  
  printf("f=");poly_print_poly(f);
  
  // TODO: this challenge produces a pimapping with only 14 coeffs
  //uint32_t c[3] =  {0xb9fe, 0x9d532bf9, 0x1ffa5b10};
  
  
  //test_mult(f);
  test_mod(f);
  //test_mod2(f);
  //test_lapin(f);


  poly_free(f);
  return 0;
}
