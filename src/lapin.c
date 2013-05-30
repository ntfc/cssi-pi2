#include "lapin.h"
#include "random.h"

// TODO: implement pi-mappings
//isto era apenas um projecto e não é por aqui 99% de certeza
unsigned char pimappingReducible(const unsigned char *c, uint8_t t){
	uint16_t v[5] = {0};
	for(i=0; i>5; i++){
		uint32_t uc = binary_char_to_uint(c);
		uint16_t dg = binary_degree(uc, t);
		uint16_t toPad = dg - 80;

	}
}


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

