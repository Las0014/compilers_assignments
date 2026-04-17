#include <stdio.h>
#include <stdint.h>

// ─────────────────────────────────────────
// Algebraic Identity
// x + 0  => x
// 0 + x  => x
// x * 1  => x
// 1 * x  => x
// ─────────────────────────────────────────
int test_algebraic(int x) {
    int a = x + 0;
    int b = 0 + a;
    int c = b * 1;
    int d = 1 * c;
    return d;
}


// ─────────────────────────────────────────
// Strength Reduction
// 15 * x  => (x << 4) - x
// x / 8   => x >> 3   (unsigned)
// ─────────────────────────────────────────
unsigned test_strength(unsigned x) {
    unsigned a = x * 15;
    unsigned b = a / 8;
    return b;
}


// ─────────────────────────────────────────
// Multi Instruction
// a = b + 1
// c = a - 1
// => c = b
// ─────────────────────────────────────────
int test_multi(int b) {
    int a = b + 1;
    int c = a - 1;
    return c;
}


// main solo per generare IR usato
int main() {
    int x = 10;

    int r1 = test_algebraic(x);
    unsigned r2 = test_strength(x);
    int r3 = test_multi(x);

    printf("%d %u %d\n", r1, r2, r3);
    return 0;
}