#include <stdio.h>
#include <math.h>

volatile double msqrt(double value)
{
    double res;
    unsigned int a = 0x4000;
    a = a >> 2;
	asm volatile (

                  "mov r1, %1\n\t"
                  "mov r2, %2\n\t"
                  "mov r3, #0\n\t"
                  "b loopif%=\n\t"

                  "loop%=:\n\t"
                  "orr r4, r3, r2\n\t"
                  "lsrs r3, r3, #1\n\t"
                  "cmp r1, r4\n\t"
                  "blt endloop%=\n\t"
                  "sub r1, r1, r4\n\t"
                  "orr r3, r3, r2\n\t"
                  "endloop%=:\n\t"
                  "lsrs r2, r2, #2\n\t"
                  "loopif%=:\n\t"
                  "cmp r5, #0\n\t"
                  "bne loop%=\n\t"

                  "end%=:\n\t"
                  "mov %0, r1\n\t"
                  : "=r" (res)
                  : "r" (value), "r" (a)
                  : "memory", "r1", "r2", "r3", "r4", "r5"
	              );
    return res;
}

int main(){

	printf("\n%lf\n%lf\n", msqrt(258), sqrt(258));

	return 0;
}
