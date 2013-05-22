#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "random-bytes.h"

//TODO: define the default word size

// polinomios sao arrays de uint32_t. uint32_t esta em stdint.h
// pode representar um polinomio (redutivel) como um array de uint16_t (para nao astar os 32bits, nao e preciso)
// assim temos: uint16_t f = { 532, 1, 0};

uint16_t f_irr_int[3] = {532, 1, 0};
// x^532 + x + 1
uint32_t f_irr_bin[17] = { 0x00100000, 0x00000000, 0x00000000, 0x00000000,
                           0x00000000, 0x00000000, 0x00000000, 0x00000000,
                           0x00000000, 0x00000000, 0x00000000, 0x00000000,
                           0x00000000, 0x00000000, 0x00000000, 0x00000000,
                           0x00000003
                         }; // x^532 + x + 1

uint8_t f_red_int[5][5] = { {127, 8, 7, 3, 0},
                            {126, 9, 6, 5, 0},
                            {125, 9, 7, 4, 0},
                            {122, 7, 4, 3, 0},
                            {121, 8, 5, 1, 0}
                          };
uint32_t f_red_bin[5][4] = { { 0x80000000, 0x00000000, 0x00000000, 0x00000189 }, // x^127+x^8+x^7+x^3+1
                             { 0x40000000, 0x00000000, 0x00000000, 0x00000261 }, // x^126+x^9+x^6+x^5+1
                             { 0x20000000, 0x00000000, 0x00000000, 0x00000291 }, // x^125+x^6+x^7+x^4+1
                             { 0x04000000, 0x00000000, 0x00000000, 0x00000099 }, // x^122+x^7+x^4+x^3+1
                             { 0x02000000, 0x00000000, 0x00000000, 0x00000123 }  // x^121+x^8+x^5+x+1
                           };

// to count the bits
static const int S[] = {1, 2, 4, 8, 16}; // Magic Binary Numbers
static const int B[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF, 0x0000FFFF};

// http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
uint32_t hamming_weight(uint32_t n) {
  unsigned int wt = 0;
  wt = n - ((n >> 1) & B[0]);
  wt = ((wt >> S[1]) & B[1]) + (wt & B[1]);
  wt = ((wt >> S[2]) + wt) & B[2];
  wt = ((wt >> S[3]) + wt) & B[3];
  wt = ((wt >> S[4]) + wt) & B[4];
  return wt;
}

// t: number of 32bit words
uint32_t degree(uint32_t* p, uint32_t t) {
  uint8_t deg = 0;
  while(*p >>= 1)
    deg++;
  // TODO: define the word size. And this muliplication can be more efficient
  return deg + (32 * (t-1));
}

// generates a double between 0 and 1
double uniform_rand() {
  // crypto secure?
  double ret = ((double)rand()/(double)RAND_MAX);
  return ret;
}

// returns a random bit
uint8_t bernoulli(double tau) {
  return (int)(uniform_rand() < tau);
}
int main() {
  srand((unsigned)time(NULL));
  uint16_t a = 0xFFFF;
  printf("%u\n", a);
  printf("U(0,1) = %.8f\n", uniform_rand());
  printf("Ber = %d\n", bernoulli(1/8));
  printf("wt(0x%x) = %d\n",f_red_bin[0][3], hamming_weight(f_red_bin[0][3]));
  printf("deg(x^532 + x + 1) = %d\n", degree(f_red_bin[1], 4));
  while(a--) printf("%d", bernoulli((double)1/(double)6));
  return 0;
}
