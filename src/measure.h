#ifndef __MEASURE_H__
#define __MEASURE_H__
#include <stdint.h>
#include <time.h>
#include "rdtsc.h"

// http://stackoverflow.com/questions/5586429/ifdef-inside-define
#define TIME_START(start) clock_gettime(CLOCK_MONOTONIC, &start)
#define TIME_END(start) clock_gettime(CLOCK_MONOTONIC, &start)
#define TIME_RESULT(start, end) ((uint32_t)time_in_ms(&end, &start))

#define CLOCK_START(start) (start = rdtsc())
#define CLOCK_END(end) (end = rdtsc())
#define CLOCK_RESULT(start, end) ((clock_cycles)(end - start))

typedef unsigned long long clock_cycles;

// http://stackoverflow.com/questions/361363/how-to-measure-time-in-milliseconds-using-ansi-c
int64_t time_in_ms(struct timespec *timeA_p, struct timespec *timeB_p);

#endif
