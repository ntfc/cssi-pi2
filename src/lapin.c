#include <stdio.h> // TODO: remove
#include "lapin.h"
#include "random.h"
#include "binary.h"
#include "poly.h"


//PiMapping irreducible
//return poly
Poly pimappingIrreducible(){
	//usa uint8_t porque cj é um número de 5 bits
	Poly p = malloc(sizeof(uint8_t) * 16);
	for(i=0; i<16; i++){
		//numero de 5 bits entre 1 e 31
		//usa o random para gerar o número entre 1 e 31
		uint8_t cj = random_uniform_range(1, 31);
		p[i] = 16 * (i-1) + cj;
	}
	return p;
}


// TODO: implement pi-mappings
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
Poly reader_step1(uint8_t n){
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
Poly tag_step2(){


}

//TODO:
//verifyication
bool reader_step3(){


}

