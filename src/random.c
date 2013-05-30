#include <stdio.h> // TODO: remove
#include <stdlib.h>
#include "random.h"

// TODO
//#ifdef URANDOM

//#elifdef SRAND

// generates a double between 0 and 1
double random_uniform() {
  double ret = ((double)rand()/(double)RAND_MAX);
  return ret;
}

// http://stackoverflow.com/a/288869/1975046
uint8_t random_uniform_range(uint8_t min, uint8_t max) {
  double ret = ((double) rand() / (((double)RAND_MAX) + 1.0)) * (max-min+1) + min;
  return (uint8_t)ret;
}

// returns a random bit
uint8_t random_bernoulli(double tau) {
  return (uint8_t)(random_uniform() < tau);
}

// TODO: should c be represented using unsigned char *?
// n: security parameter in bits
// returns an array with 3 elements. 16 leftmost bits are set to 0
u_char* random_gen_c(uint8_t n) {
  uint8_t words = n/8; // convert SEC_PARAM to bytes
  u_char *c = calloc(words, sizeof(u_char)); // uchar = 1byte
  uint8_t i = 0;
  printf("words = %u\n", words);
  for(i = 0; i < words; i++) {
    c[i] = random_uniform_range(0x0, 0xF);
  }
  return c;
}

//#endif
