#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "lapin.h"
#include "poly.h"

void usage() {
  fprintf(stderr, "Usage: \n");
}

/*int main(int argc, char **argv) {
  // TODO: rudimentar parsing...
  // -g: save keys to file
  // -c: generate challenge to file
  // -m: create mac and save to file
  // -v: vrfy and return result to stdio
  
  if(argv[1] != '-g' || argv[1] != 'c' || argv[1] != 'm' || argv[1] != 'v') {
    
  }
  
}*/

int main(int argc, char **argv) {
  srand((unsigned)time(NULL));

  Poly *b1 = poly_rand_uniform_poly(150);
  Poly *b2 = poly_rand_uniform_poly(150);
  poly_print_poly(b1);
  poly_print_poly(b2);
  PolyCRT *ber1 = poly_to_crt(b1, f_reducible_crt);
  PolyCRT *ber2 = poly_to_crt(b2, f_reducible_crt);
  poly_crt_print_poly(ber1);
  poly_crt_print_poly(ber2);

  PolyCRT *mult = poly_crt_mult(ber1, ber2, f_reducible_crt);
  poly_crt_print_poly(mult);
  
  
  poly_free(b1);
  poly_free(b2);
  poly_crt_free(ber1);
  poly_crt_free(ber2);
  poly_crt_free(mult);

  /*Lapin *lapin = lapin_init(IRREDUCIBLE);
  Challenge c = challenge_generate(lapin->sec_param);
  Poly *r, *z;
  int8_t tag = lapin_tag(lapin, c, &r, &z);
  int8_t vrfy = lapin_vrfy(lapin, c, r, z);
  printf("Vrfy (r,z) = %u\n", vrfy);
  poly_print_poly(r);
  poly_print_poly(z);
  Poly *aa = poly_mult(r,z);
  poly_print_poly(aa);
  Poly *bb = poly_mod(aa, f_irreducible, NULL); 
  poly_print_poly(bb);
  lapin_end(lapin);
  

  
  challenge_free(c);
  poly_free(r);
  poly_free(z);*/
  
  return 0;
}

