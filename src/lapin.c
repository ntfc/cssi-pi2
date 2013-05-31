#include <stdio.h> // TODO: remove
#include <stdlib.h>
#include "lapin.h"
#include "random.h"
#include "binary.h"

// n: security parameter in bits
// NOTE: Challenge must be free'd in the end
Challenge lapin_gen_c(uint8_t n) {
  uint8_t words = CEILING(((double)n/(double)8) / (double)sizeof(uint32_t)); // convert SEC_PARAM to bytes
  printf("w = %u\n", words);
  Challenge c = calloc(words, sizeof(uint32_t));
  uint8_t i = 0;
  
  uint8_t pad = (words * W) - n;
  
  for(i = 0; i < words; i++) {
    c[i] = random_uniform_uint32();
  }
  printf("pad = %u\n", pad);
  c[0] &= (0xFFFFFFFF >> pad);
  return c;
}

//PiMapping irreducible
//return poly
Poly* lapin_pimapping_irreduc(const Challenge c, uint8_t n) {
  int8_t j = 0, k = 0, i = 0;
  uint8_t cj;
  Poly *p;
  uint8_t words = CEILING(((double)n/(double)8) / (double)sizeof(*c)); // ceil((n/8) / 4)

  // tmp copy of c
  Challenge tmpC = calloc(words, sizeof(*c)); // TODO: does it make sense?
  for(j = 0; j < words; j++)
    tmpC[j] = c[j];
    
  uint8_t coeffs[16];
  for(i = 16 - 1, j = 0; i >= 0; i--, j++) {
    cj = tmpC[words - 1] & 0x1F;    
    // here we use j instead of the (j-1) indicated in the paper because
    //we're in 0-index mode
    coeffs[j] = 0xF * j + cj;
    // now, bj is a poly coefficient
    // shift tmpC
    k = 5; // 0x1F is 5 bits
    while(k--) {
      binary_array_shift_right(tmpC, words);
    }    
  }
  free(tmpC);
  p = poly_create_poly_from_coeffs(coeffs, 16);
  return p;
}


/*// TODO: implement pi-mappings
// c must be an array with 80 elems
void lapin_pimapping_reduc(const unsigned char *c) {
  uint8_t i = 0;
  uint8_t toPad = 0;
  for(i = 0; i < 5; i++) {
    toPad = binary_degree(F_PROD_REDUCIBLE[i], 4) - SEC_PARAM;
    printf("%d\n", toPad);
  }
}

//TODO: confirmar isto
//generate c
//uint8_t security parameter
Poly lapin_reader_step1(uint8_t n){
  //n/8 é o número de palavras??
  // convert SEC_PARAM to bytes
  uint8_t w = n/8;
  Poly c = malloc(sizeof(uint32_t) * w);
  u_char* genc = random_gen_c(uint8_t n);
  for(i = 0; i < w; i++) {
      uint32_t u = binary_char_to_uint(genc[i]);
      c[i] = u;
    }
    return c;
}

//TODO:
//generate r, e
//calculate z
Poly lapin_tag_step2(){


}

//TODO:
//verifyication
void lapin_reader_step3(){


}*/
