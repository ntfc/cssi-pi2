#ifndef __FILE_H__
#define __FILE_H__

#include <stdio.h>
#include "poly.h"
#include "lapin.h"

int8_t file_save_challenge(FILE *fp, const Challenge c);
Challenge file_read_challenge(FILE *fp);

int8_t file_save_poly(FILE *fp, const Poly *p);
Poly* file_read_poly(FILE *fp);

int8_t file_save_crt(FILE *fp, const PolyCRT *c);
PolyCRT* file_read_crt(FILE *fp);

// return the number of read polys
int8_t file_read_polys(FILE *fp, Poly ***ps);

#endif
