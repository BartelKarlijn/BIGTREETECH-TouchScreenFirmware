#ifndef _MY_MISC_H_
#define _MY_MISC_H_


// Macros to make a string from a macro
#define STRINGIFY_(M) #M
#define STRINGIFY(M) STRINGIFY_(M)

#define COUNT(n) (sizeof(n)/sizeof(n[0]))

#define ABS(n) ((n) > 0 ? (n) : -(n))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#define NOBEYOND(min, v, max) MAX(min, MIN(v, max))


uint8_t inRange(int cur, int tag , int range);
long map(long x, long in_min, long in_max, long out_min, long out_max);

#endif
