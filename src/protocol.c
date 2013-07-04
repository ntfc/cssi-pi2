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

  Lapin *lapin = lapin_init(IRREDUCIBLE);
  Challenge c = challenge_generate(lapin->sec_param);
  printf("s1=");poly_print_poly(lapin->key->s1);
  printf("s2=");poly_print_poly(lapin->key->s2);
  printf("c=");challenge_print_challenge(c);
  //PolyCRT *r_crt, *z_crt;
  Poly *r, *z;
  lapin_tag(lapin, c, &r, &z);
  //poly_crt_print_poly(r_crt);
  //poly_crt_print_poly(z_crt);
  printf("r=");poly_print_poly(r);
  printf("z=");poly_print_poly(z);
  printf("vrfy = %u\n", lapin_vrfy(lapin, c, r, z));
  
  poly_free(r);
  poly_free(z);
  challenge_free(c);
  lapin_end(lapin);
  
  //PolyCRT *rr = poly_crt_rand_uniform(lapin->f.crt);
  //poly_crt_print_poly(r);
  /*Poly *r, *z;
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

