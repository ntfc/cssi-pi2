#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "random-bytes.h"

#define CHAR_BIT_SIZE sizeof(char) * 8 // should always be equal to 8
#define W (uint8_t)(sizeof(uint32_t) * 8)
#define NUMBER_OF_WORDS(m) CEILING((double)m / (double)W)

// http://stackoverflow.com/a/6556588/1975046
#define CEILING_POS(X) ((X-(int)(X)) > 0 ? (int)(X+1) : (int)(X))
#define CEILING_NEG(X) ((X-(int)(X)) < 0 ? (int)(X-1) : (int)(X))
#define CEILING(X) ( ((X) > 0) ? CEILING_POS(X) : CEILING_NEG(X) )

// TODO: move the random functions to a module random.h
// TODO: check http://crypto.stackexchange.com/questions/8388/polynomial-multiplication-and-division-in-2128
// TODO: define the default word size
// TODO: add a method to create a random polynomial using bernoulli and uniform_rand. convert it to char*
// TODO: dont convert the char* to uint at once. Convert every 32chars to a uint
// NOTE: use the urandom to create the keys and random polynomials => random reducible poly == genreate 4 random uint32_t, 5 times
// TODO: organize this :)
// NOTE: printf unsigned char = %u

typedef uint32_t* Poly;

// polinomios sao arrays de uint32_t. uint32_t esta em stdint.h

// representacao mais agradavel
uint16_t f_irr_int[3] = {532, 1, 0};
// representacao binaria, mais eficiente
uint32_t f_irr_bin[17] = { 0x00100000, 0x00000000, 0x00000000, 0x00000000,
                           0x00000000, 0x00000000, 0x00000000, 0x00000000,
                           0x00000000, 0x00000000, 0x00000000, 0x00000000,
                           0x00000000, 0x00000000, 0x00000000, 0x00000000,
                           0x00000003
                         }; // x^532 + x + 1
// representacao mais agradavel
uint8_t f_red_int[5][5] = { {127, 8, 7, 3, 0},
                            {126, 9, 6, 5, 0},
                            {125, 9, 7, 4, 0},
                            {122, 7, 4, 3, 0},
                            {121, 8, 5, 1, 0}
                          };
// representacao binaria, mais eficiente
uint32_t f_red_bin[5][4] = { { 0x80000000, 0x00000000, 0x00000000, 0x00000189 }, // x^127+x^8+x^7+x^3+1
                             { 0x40000000, 0x00000000, 0x00000000, 0x00000261 }, // x^126+x^9+x^6+x^5+1
                             { 0x20000000, 0x00000000, 0x00000000, 0x00000291 }, // x^125+x^6+x^7+x^4+1
                             { 0x04000000, 0x00000000, 0x00000000, 0x00000099 }, // x^122+x^7+x^4+x^3+1
                             { 0x02000000, 0x00000000, 0x00000000, 0x00000123 }  // x^121+x^8+x^5+x+1
                           };

// to count the bits
// Used by: http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
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

// p: 1st word in the polynomial // ATTENTION: by 1st, we mean the MSB
// t: number of 32bit words in the polynomial
uint32_t degree(uint32_t* p, uint32_t t) {
  uint8_t deg = 0;
  while(*p >>= 1)
    deg++;
  // TODO: define the word size. And this muliplication can be more efficient
  return deg + (W * (t-1));
}

// reverses all bits from a number: reverse_number(0b01001) = 0b10010
uint32_t reverse_number(uint32_t n) {
  uint32_t s = sizeof(n) * CHAR_BIT_SIZE; // bit size; must be power of 2 
  uint32_t mask = ~0;         
  while ((s >>= 1) > 0) {
    mask ^= (mask << s);
    n = ((n >> s) & mask) | ((n << s) & ~mask);
  }
  return n;
}

// generates a double between 0 and 1
double uniform_rand() {
  double ret = ((double)rand()/(double)RAND_MAX);
  return ret;
}

uint8_t uniform_rand_range(uint8_t min, uint8_t max) {
  double ret = ((double) rand() / (((double)RAND_MAX) + 1.0)) * (max-min+1) + min;
  return (uint8_t)ret;
}

// returns a random bit
uint8_t bernoulli(double tau) {
  return (uint8_t)(uniform_rand() < tau);
}

// generate a random word, according to bernoulli distribution
uint32_t random_bernoulli_32bit_word(double tau) {
  unsigned char word[W] = {0};
  int i = 0;
  for(; i < W; i++) {
    // convert (int)0 to '0' or (int)1 to '1'
    word[i] = '0'+bernoulli(tau);
  }
  return char_to_uint(word);
}

// f: polynomial of degree m
// t: number of words in f
uint32_t* gen_bernoulli_rand_poly(uint32_t* f, uint32_t t, double tau) {
  // TODO: generate a random poly using the bernoulli distribution
  // TODO: return uint32_t* or char* ?
  //uint32_t i = 0;
  uint8_t j = 0;
  //uint32_t m = degree(f, t);
  uint8_t new_word[W];
  //for(; i < m; i++) {
    // generate one 32-bit word
    for(j = 0; j < W; j++) {
      //printf("%u\n", (char)bernoulli(tau));
      new_word[j] = bernoulli(tau);
    }
  //}
  return NULL;
}

/*uint32_t uint_array_to_uint(uint32_t *t) {
  // TODO: receive uint32_t* or char*?
  uint32_t n = 0;
  while(*t++) {
    printf("->%d\n", *t);
    / *n <<= 1;
    if(*t++ == 1)
      n ^= 1;* /
  }
  return n;
}*/

// convert a binary string to uint32_t
// bits from position 32 forward are discarded
uint32_t char_to_uint(const unsigned char *c) {
  uint32_t out = 0;
  const unsigned char *s = c;
  while(*s && s-c < W) {
    out = (out << 1) | (*s == '0' ? 0 : 1);
    s++;
  }
  return out;
}

// print 32 chars from a string
void test_print_char_word(const unsigned char *c) {
  const unsigned char *s = c;
  while(*c && c-s < W) {
    printf("%c", *c);
    c++;
  }
  printf("\n");
}

// generate a random number between
uint32_t random_uniform_32bit_word() {
  unsigned char word[W] = {0};
  int i = 0;
  for(; i < W; i++) {
    // convert (int)0 to '0' or (int)1 to '1'
    word[i] = '0'+uniform_rand_range(0, 1);
  }
  return char_to_uint(word);
}

// TODO: generate a poly smaller than f
// m words
Poly random_poly(uint8_t m) {
  Poly p = malloc(sizeof(uint32_t) * m);
  
  while(m--)
    p[m] = random_uniform_32bit_word();
  return p;
}

//uint32_t* random_ring_element(uint32_t* f, uint32_t m) {
  
//}

int main() {
  srand((unsigned)time(NULL));
  //printf("U(0,1) = %.8f\n", uniform_rand());
  //printf("Ber = %d\n", bernoulli(1/8));
  //printf("wt(0x%x) = %d\n",f_red_bin[0][3], hamming_weight(f_red_bin[0][3]));
  //printf("deg(x^532 + x + 1) = %d\n", degree(f_irr_bin, 17));
  //printf("%p\n", gen_bernoulli_rand_poly(f_irr_bin, 17, (double)1/(double)8));
  //memcpy(b, gen_bernoulli_rand_poly(f_irr_bin, 17, (double)1/(double)8), W);
  Poly c = random_poly(4);
  printf("0x%u 0x%u 0x%u 0x%u\n", c[0], c[1], c[2], c[3]);
  return 0;
}
