#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "lapin.h"
#include "poly.h"

void usage() {
  fprintf(stderr, "Usage: \n");
}

void test_lapin_reduc() {
  Lapin *lapin = lapin_init(REDUCIBLE);
  Challenge c = challenge_generate(lapin->sec_param);
  printf("s1=");poly_crt_print_poly(lapin->key_crt->s1);
  printf("s2=");poly_crt_print_poly(lapin->key_crt->s2);
  printf("c=");challenge_print_challenge(c);
  PolyCRT *pi = lapin_pimapping_reduc(lapin, c);
  printf("pi=");poly_crt_print_poly(pi);
  poly_crt_free(pi);

  PolyCRT *r, *z;
  lapin_tag(lapin, c, &r, &z);
  printf("r=");poly_crt_print_poly(r);
  printf("z=");poly_crt_print_poly(z);
  // not working yet
  //printf("vrfy = %u\n", lapin_vrfy(lapin, c, r, z));
  
  poly_crt_free(r);
  poly_crt_free(z);
  challenge_free(c);
  lapin_end(lapin);
}

void test_lapin_irreduc() {
  Lapin *lapin = lapin_init(IRREDUCIBLE);
  Challenge c = challenge_generate(lapin->sec_param);
  printf("s1=");poly_print_poly(lapin->key->s1);
  printf("s2=");poly_print_poly(lapin->key->s2);
  printf("c=");challenge_print_challenge(c);
  
  Poly *r, *z;
  lapin_tag(lapin, c, &r, &z);
  
  printf("r=");poly_print_poly(r);
  printf("z=");poly_print_poly(z);
  printf("vrfy = %u\n", lapin_vrfy(lapin, c, r, z));
  challenge_free(c);
  lapin_end(lapin);
}

int main(int argc, char **argv) {
  srand((unsigned)time(NULL));

  test_lapin_reduc();
  //test_lapin_irreduc();
  
  return 0;
}

