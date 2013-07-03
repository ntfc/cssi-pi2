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
  poly_free(z);
  
  return 0;
}
