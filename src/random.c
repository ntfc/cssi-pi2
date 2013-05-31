#include <stdio.h> // TODO: remove
#include <stdlib.h>
#include "random.h"
#include "binary.h"

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
//#endif

// Generate a uniform random uint32
uint32_t random_uniform_uint32(void) {
  uint8_t i = 0;
  uint32_t n = 0;
  for(i = 0; i < 32; i++) {
    n <<= 1;
    n ^= random_uniform_range(0, 1);
  }
  return n;
}

// generate a random uint32, according to bernoulli distribution
uint32_t random_bernoulli_uint32(double tau) {
  uint8_t i = 0;
  uint32_t n = 0;
  for(; i < 32; i++) {
    n <<= 1;
    n ^= random_bernoulli(tau);
  }
  return n;
}

// n: security parameter in bits
// returns an array with 3 elements. 16 leftmost bits are set to 0
// NOTE: Challenge must be free'd in the end
Challenge random_gen_c(uint8_t n) {
  uint8_t words = n/8; // convert SEC_PARAM to bytes
  Challenge c = calloc(words, sizeof(uint32_t));
  uint8_t i = 0;
  
  uint8_t pad = n - (words * W);
  
  for(i = 0; i < words; i++) {
    c[i] = random_uniform_uint32();
  }
  c[0] &= (0xFFFFFFFF >> pad);
  return c;
}
