#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "lapin.h"
#include "poly.h"
#include "random.h"
#include "measure.h"

void usage() {
  fprintf(stderr, "Usage: \n");
}

void test_lapin_reduc() {
  Lapin *lapin = lapin_init(REDUCIBLE);
  Challenge c = challenge_generate(lapin->sec_param);
  #ifdef DEBUG
  printf("s1=");poly_crt_print_poly(lapin->key_crt->s1);
  printf("s2=");poly_crt_print_poly(lapin->key_crt->s2);
  printf("c=");challenge_print_challenge(c);
  #endif
  PolyCRT *pi = lapin_pimapping_reduc(lapin, c);
  #ifdef DEBUG
  printf("pi=");poly_crt_print_poly(pi);
  #endif
  poly_crt_free(pi);

  PolyCRT *r, *z;
  lapin_tag(lapin, c, &r, &z);
  #ifdef DEBUG
  printf("r=");poly_crt_print_poly(r);
  printf("z=");poly_crt_print_poly(z);
  #endif
  // not working yet
  //printf("reduc vrfy = %u\n", lapin_vrfy(lapin, c, r, z));
  
  poly_crt_free(r);
  poly_crt_free(z);
  challenge_free(c);
  lapin_end(lapin);
}

void test_lapin_irreduc() {
  Lapin *lapin = lapin_init(IRREDUCIBLE);
  Challenge c = challenge_generate(lapin->sec_param);
  #ifdef DEBUG
  printf("s1=");poly_print_poly(lapin->key->s1);
  printf("s2=");poly_print_poly(lapin->key->s2);
  printf("c=");challenge_print_challenge(c);
  #endif
  Poly *r, *z;
  lapin_tag(lapin, c, &r, &z);
  #ifdef DEBUG
  printf("r=");poly_print_poly(r);
  printf("z=");poly_print_poly(z);
  #endif
  printf("irreduc vrfy = %u\n", lapin_vrfy(lapin, c, r, z));
  poly_free(r);
  poly_free(z);
  challenge_free(c);
  lapin_end(lapin);
}

int main(int argc, char **argv) {
  srand((unsigned)time(NULL));

  //test_lapin_reduc();
  test_lapin_irreduc();
  Poly *a = poly_rand_uniform_poly(532);
  Poly *b = poly_rand_uniform_poly(532);
  Poly *c = poly_mult(a, b);
  poly_free(a); poly_free(b);
  printf("c=");poly_print_poly(c);
  clock_cycles a1, a2, b1, b2;
  CLOCK_START(a1);
  Poly *cMod = poly_mod(c, f_irreducible);
  CLOCK_END(b1);
  CLOCK_START(a1);
  Poly *cMod2 = poly_fast_mod_irreduc(c, f_irreducible);
  CLOCK_END(b2);
  printf("c1=");poly_print_poly(cMod);
  printf("c2=");poly_print_poly(cMod2);
  printf("mod1 = %llu\n", CLOCK_RESULT(a1, b1));
  printf("mod2 = %llu\n", CLOCK_RESULT(a2, b2));
  return 0;
}

