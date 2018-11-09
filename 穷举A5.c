#include <stdio.h>
#include <stdlib.h>

#if 2147483647L+1L == -2147483648L
typedef long long32;
typedef unsigned long ulong32;
#else
typedef int long32;           /* In 64-bit systems, long may be 64-bit, */
typedef unsigned int ulong32; /* here we force it to be 32-bit. */
#endif
//total bit
#define R1MASK  0x07FFFF
#define R2MASK  0x3FFFFF
#define R3MASK  0x7FFFFF
//clock control bit
#define R1MID   0x000100 /* bit 8 */
#define R2MID   0x000400 /* bit 10 */
#define R3MID   0x000400 /* bit 10 */
//xor bit
#define R1TAPS  0x072000 /* bits 18,17,16,13 */
#define R2TAPS  0x300000 /* bits 21,20 */
#define R3TAPS  0x700080 /* bits 22,21,20,7 */
//output bit
#define R1OUT   0x040000 /* bit 18 (the high bit) */
#define R2OUT   0x200000 /* bit 21 (the high bit) */
#define R3OUT   0x400000 /* bit 22 (the high bit) */

ulong32 R1, R2, R3;


ulong32 XOR(ulong32 x) {
    x ^= x >> 16;
    x ^= x >> 8;
    x ^= x >> 4;
    x ^= x >> 2;
    x ^= x >> 1;
    return x & 1;
}


ulong32 clockone(ulong32 reg, ulong32 mask, ulong32 taps) {
    ulong32 t = reg & taps;
    reg = (reg << 1) & mask;
    reg |= XOR(t);
    return reg;
}

void clock() {
    ulong32 x1 = ((R1 & R1MID) >> 8) & 1;
    ulong32 x2 = ((R2 & R2MID) >> 10) & 1;
    ulong32 x3 = ((R3 & R3MID) >> 10) & 1;
    ulong32 maj = (x1 + x2 + x3) >= 2 ? 1 : 0; //count majority
    if (x1  == maj) {
        R1 = clockone(R1, R1MASK, R1TAPS);
    }
    if (x2 == maj) {
        R2 = clockone(R2, R2MASK, R2TAPS);
    }
    if (x3 == maj) {
        R3 = clockone(R3, R3MASK, R3TAPS);
    }
}

int main(void) {
    unsigned long long i = 0;
    printf("sizeof int:%d sizeof long:%d sizeof long long:%d\n", sizeof(int), sizeof(long), sizeof(long long));
    ulong32 initial1, initial2, initial3;
    initial1 = R1 = 0b011;
    initial2 = R2 = 0b01;
    initial3 = R3 = 0b111;
    printf("R1:%lu R2:%lu R3:%lu \n", R1, R2, R3);
    do {
        i++;
        clock();
        if (i % 10000000 == 0) {
            printf("%llu: %lu %lu %lu\n", i, R1, R2, R3);
        }
    } while (! ((R1 == initial1) && (R2 = initial2 ) && (R3 == initial3)));
    printf("%llu: %lu %lu %lu\n", i, R1, R2, R3);
    return 0;
}



