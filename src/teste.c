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
  if(!f) {
    fprintf(stderr, "ERROR poly NULL\n");
    return;
  }
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
  Poly *d = poly_mod(c, f, &table);
  printf("c mod=");poly_print_poly(d);
  poly_free(a);
  poly_free(b);
  poly_free(c);
  poly_free(d);
  poly_free_table(table, W);
}


uint32_t** test_table(const Poly *f) {
  uint8_t k = 0;
  uint16_t i = 0;
  uint32_t **table; // 2-dimension array = table
  // one more word than f
  uint16_t t = f->t + 1; // number of words in each u_k
  table = malloc(sizeof(uint32_t*) * W);
  
  uint32_t *u0 = calloc(t, sizeof(uint32_t));
  // copy f->vec to u0. u0[0] = 0x0, u0[1] = f->vec[0], etc
  for(i = 1; i < t; i++) {
    u0[i] = f->vec[i-1];
  }
  table[0] = u0;
  
  k = 1;
  while(k < W) {
    table[k] = calloc(t, sizeof(uint32_t));
    binary_array_shift_left2(table[k - 1], t, table[k]);
    k++;
  }
  
  // print table  
  /*for(k = 0; k < W; k++) {
    printf("u[%u] = ", k);
    for(i = 0; i < t; i++) {
      printf("0x%.8x ", table[k][i]);
    }
    printf("\n");
  }*/
  return table;
}

void test_mod3(const Poly *f) {
  // a = x^231 + x^230 + x^229 + x^227 + x^224 + x^221 + x^215 + x^213 + x^210 + x^209 + x^207 + x^206 + x^204 + x^200 + x^199 + x^198 + x^194 + x^193 + x^192 + x^191 + x^190 + x^188 + x^187 + x^183 + x^178 + x^171 + x^169 + x^167 + x^166 + x^164 + x^163 + x^162 + x^158 + x^156 + x^155 + x^152 + x^150 + x^146 + x^144 + x^143 + x^142 + x^141 + x^138 + x^137 + x^136 + x^135 + x^133 + x^130 + x^127 + x^125 + x^124 + x^123 + x^122 + x^121 + x^117 + x^115 + x^113 + x^110 + x^105 + x^104 + x^103 + x^102 + x^101 + x^98 + x^95 + x^93 + x^92 + x^91 + x^89 + x^87 + x^84 + x^82 + x^81 + x^80 + x^78 + x^75 + x^72 + x^69 + x^66 + x^65 + x^63 + x^62 + x^59 + x^58 + x^54 + x^51 + x^50 + x^47 + x^46 + x^43 + x^42 + x^40 + x^38 + x^36 + x^34 + x^32 + x^29 + x^28 + x^27 + x^26 + x^25 + x^19 + x^14 + x^11 + x^8 + x^5 + x^3 + x
  //uint16_t aCoeffs[108] = {1, 3, 5, 8, 11, 14, 19, 25, 26, 27, 28, 29, 32, 34, 36, 38, 40, 42, 43, 46, 47, 50, 51, 54, 58, 59, 62, 63, 65, 66, 69, 72, 75, 78, 80, 81, 82, 84, 87, 89, 91, 92, 93, 95, 98, 101, 102, 103, 104, 105, 110, 113, 115, 117, 121, 122, 123, 124, 125, 127, 130, 133, 135, 136, 137, 138, 141, 142, 143, 144, 146, 150, 152, 155, 156, 158, 162, 163, 164, 166, 167, 169, 171, 178, 183, 187, 188, 190, 191, 192, 193, 194, 198, 199, 200, 204, 206, 207, 209, 210, 213, 215, 221, 224, 227, 229, 230, 231 };
  // b = x^232 + x^231 + x^230 + x^229 + x^225 + x^224 + x^222 + x^219 + x^218 + x^216 + x^215 + x^214 + x^213 + x^212 + x^211 + x^208 + x^206 + x^205 + x^204 + x^199 + x^198 + x^196 + x^194 + x^193 + x^192 + x^191 + x^190 + x^187 + x^184 + x^183 + x^181 + x^180 + x^178 + x^177 + x^174 + x^173 + x^172 + x^169 + x^168 + x^162 + x^160 + x^159 + x^158 + x^155 + x^153 + x^151 + x^150 + x^149 + x^148 + x^144 + x^139 + x^138 + x^137 + x^136 + x^134 + x^129 + x^125 + x^124 + x^123 + x^118 + x^117 + x^115 + x^113 + x^111 + x^109 + x^106 + x^102 + x^100 + x^99 + x^97 + x^95 + x^94 + x^93 + x^92 + x^87 + x^85 + x^84 + x^79 + x^78 + x^73 + x^70 + x^66 + x^65 + x^63 + x^60 + x^58 + x^57 + x^55 + x^54 + x^53 + x^51 + x^50 + x^49 + x^48 + x^47 + x^43 + x^42 + x^41 + x^39 + x^35 + x^34 + x^32 + x^31 + x^29 + x^28 + x^26 + x^25 + x^24 + x^22 + x^20 + x^19 + x^18 + x^17 + x^13 + x^9 + x^5 + x^4 + x^2
  //uint16_t bCoeffs[118] = {2, 4, 5, 9, 13, 17, 18, 19, 20, 22, 24, 25, 26, 28, 29, 31, 32, 34, 35, 39, 41, 42, 43, 47, 48, 49, 50, 51, 53, 54, 55, 57, 58, 60, 63, 65, 66, 70, 73, 78, 79, 84, 85, 87, 92, 93, 94, 95, 97, 99, 100, 102, 106, 109, 111, 113, 115, 117, 118, 123, 124, 125, 129, 134, 136, 137, 138, 139, 144, 148, 149, 150, 151, 153, 155, 158, 159, 160, 162, 168, 169, 172, 173, 174, 177, 178, 180, 181, 183, 184, 187, 190, 191, 192, 193, 194, 196, 198, 199, 204, 205, 206, 208, 211, 212, 213, 214, 215, 216, 218, 219, 222, 224, 225, 229, 230, 231, 232 };
  //Poly *a = poly_create_poly_from_coeffs(f, aCoeffs, 108);
  //Poly *b = poly_create_poly_from_coeffs(f, bCoeffs, 118);
  Poly *a = poly_rand_uniform_poly(f);
  Poly *b = poly_rand_uniform_poly(f);
  Poly *c = poly_mult(a, b);
  //Poly *r = poly_get_r(f);
  printf("a=");poly_print_poly(a);
  printf("b=");poly_print_poly(b);
  printf("c=");poly_print_poly(c);
  //printf("r=");poly_print_poly(r);
  // pre-compute table
  uint32_t **table = test_table(f);
  
  uint16_t j, i;
  uint8_t k;
  uint16_t m = f->m;
  uint16_t j_aux;

  // TODO: validate c->t and c->m  
  Poly *C = poly_alloc(c->m);
  
  // copy c->vec to C->vec
  for(i = 0; i < c->t; i++) {
    C->vec[i] = c->vec[i];
  }
  
  for(i = (2 * f->m) - 2; i >= f->m; i--) {
    uint8_t ci = poly_get_bit(C, i);
    if(ci == 1) {
      j = floor(((double)i - (double)m) / (double)W);
      
      k = (i - m) - (W * j);
      //printf("c->m = %u, c->t = %u, f->t = %u, r->t = %u\n", c->m, c->t, f->t, r->t);
      
      j_aux = j;
      while( (j_aux < C->t) && ((j_aux - j) <= f->t)) {
         // C{j_aux} = C{j_aux} ^ u[k]
        uint16_t ci_i = GET_VEC_WORD_INDEX(C->t, j_aux);
        uint32_t ci = C->vec[ci_i];
        
        uint32_t uk_k = GET_VEC_WORD_INDEX(f->t+1, j_aux - j);
        uint32_t uk = table[k][uk_k];

        C->vec[ci_i] = ci ^ uk;
        //printf("%.8x\n", C->vec[ci_i]);
        
        j_aux++;
      }
    }
    
  }

  Poly *cMod = poly_alloc(f->m);
  for(i = 0; i < cMod->t; i++) {
    cMod->vec[(cMod->t - 1) - i] = C->vec[(C->t - 1) - i];
  }
  //cMod->vec[0] &= (0xffffffff >> cMod->s); // align last word
  printf("c mod = ");poly_print_poly(cMod);
  //poly_free(r);
  poly_free(C);
}

void test_lapin_irr(const Poly *f) {
  Challenge c = challenge_generate(SEC_PARAM);
  int i = 0;
  unsigned char w[32+1];
  /*printf("c=");
  for(i = 0; i <= ceil(SEC_PARAM/W); i++)
    printf("%s", binary_uint32_to_char(c[i], w));
  printf("\n");
  printf("0x%.8x\n", random_uniform_uint32());*/
  Key *key = key_generate(f);
  /*printf("s1=");poly_print_poly(key->s1);
  printf("s2=");poly_print_poly(key->s2);*/
    
  Poly *r, *z;
  uint32_t **table = NULL;
  lapin_tag_step2(key, f, c, &z, &r, (double)1/(double)8, SEC_PARAM, &table);
  
  /*printf("r=");poly_print_poly(r);
  printf("z=");poly_print_poly(z);*/
  
  int vrfy = lapin_reader_step3(key, f, c, z, r, (double)0.27, SEC_PARAM, &table);
  printf("Vrfy = %d\n", vrfy);
  
  
  // faltam fazer 4 free's
  poly_free(z);
  poly_free(r);
  challenge_free(c);
  key_free(key);
  poly_free_table(table, W);
}

void test_pi_irr(const Poly *f) {
  uint32_t c[3] = {0x3c49, 0xf7edb6e0, 0xbba10de5};
  int i = 0;
  unsigned char w[32+1];
  printf("c=");
  for(i = 0; i <= ceil(SEC_PARAM/W); i++)
    printf("%s", binary_uint32_to_char(c[i], w));
  printf("\n");
  Poly *pi = lapin_pimapping_irreduc(f, c, SEC_PARAM);
  printf("pi=");poly_print_poly(pi);
}

// NOTE: a and b can have diffent lengths..
// right-to-left comb method
Poly* test_poly_mult(const Poly *a, const Poly *b) {
  const Poly *bigger = a, *smaller = b;
  if(a->t < b->t) {
    bigger = b;
    smaller = a;
  }
  uint16_t j, i;
  uint8_t k;
  uint16_t t = bigger->t;
  uint16_t m = bigger->m;
  uint16_t c_max_degree = (a->m + b->m) - 1;
  
  uint16_t c_words = (uint16_t)ceil((double)c_max_degree / (double)W); // number of words in C
  //uint8_t c_unused = W*c_words - c_max_degree;  
  Poly *c = poly_alloc(c_max_degree);
  // B is a copy of b, but with one more word
  uint32_t *B = calloc(smaller->t + 1, sizeof(uint32_t));
  // b[0] is already zero'd
  for(j = 0; j < smaller->t; j++) {
    B[j+1] = b->vec[j];
  }
  for(k = 0; k < W; k++) {
    for(j = 0; j < t; j++) {
      // get k-th bit of A[j]
      if(binary_get_bit(a->vec[GET_VEC_WORD_INDEX(a->t, j)], k) == 1) {
        i = j;
        // C{j} = C{j} + B
        while((i < c_words) && ((i - j) <= t)) {
          // C[i] = C[i] ^ B[i - j]
          c->vec[GET_VEC_WORD_INDEX(c_words, i)] ^= B[GET_VEC_WORD_INDEX(b->t + 1, i - j)];
          i++;
        }
      }
    }
    if(k != W - 1) {
      // B = B * x
      binary_array_shift_left(B, smaller->t+1);
    }
  }
  // free B
  free(B);
  return c;
}

int main() {
  srand((unsigned)time(NULL));
  
  Poly *f = poly_alloc(532);
  poly_set_coeffs_from_uint32(f, F_IRREDUCIBLE);
  //printf("f=");poly_print_poly(f);
  
  /*// x^233 + x^74 + 1
  uint32_t new_f_coefs[] = {0x200, 0x0, 0x0, 0x0, 0x0, 0x400, 0x0, 0x1};
  Poly *f2 = poly_alloc(233);
  poly_set_coeffs_from_uint32(f2, new_f_coefs);
  printf("f2=");poly_print_poly(f2);*/
  
  // TODO: this challenge produces a pimapping with only 14 coeffs
  //uint32_t c[3] =  {0xb9fe, 0x9d532bf9, 0x1ffa5b10};
  
  
  //test_mult(f);
  //test_mod(f);
  //test_mod2(f2);
  //test_mod(f);
  uint8_t i = 0;
  PolyVec ff[5];
  while(i < 5) {
    Poly *fi = poly_create_irreduc(F_PROD_REDUCIBLE_M[i], F_PROD_REDUCIBLE_R[i], 4);
    printf("fi = ");poly_print_poly(fi);
    ff[i++] = fi;
  }
  i = 0;
  Poly *new_f = poly_mult(ff[0], ff[1]);
  //printf("new_f=");poly_print_poly(new_f);
  
  
  Poly *a = poly_rand_uniform_poly(new_f);
  Poly *b = poly_rand_uniform_poly(f);
  printf("a="); poly_print_poly(a);
  printf("b="); poly_print_poly(b);
  Poly *test_mult = poly_add(a, b);
  printf("test_mult=\n");poly_print_poly(test_mult);
  //test_lapin_irr(f);
  //test_lapin_red(f);
  //test_pi_irr(f);


  poly_free(f);
  return 0;
}
