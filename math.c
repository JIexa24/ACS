#include <stdio.h>
#include <math.h>

volatile int mpow(int num, int radix)
{
  int ret = -1;
  asm volatile (
  /*AT&T syntax*/
	        "mov r0, #1\n\t"
//                "movl $1, %%eax\n\t"
	        "mov r1, %1\n\t"
	        "mov r2, %2\n\t"
	        "cmp $0, r2\n\t"
                "bl lowpow%=\n\t"
                "beq endpow%=\n"
              "begpow%=:\n\t"
                "cmpl $0, r2\n\t"
                "beq endpow%=\n\t"
                "mul r0, r0, r1\n\t"
                "sub r2, r2, #1\n\t"
                "b begpow%=\n"
              "lowpow%=:\n\t"
                "mov r0, #0\n"
              "endpow%=:\n\t"
                "mov %0, r0\n"
                : "=r" (ret)
                : "r" (num), "r" (radix)
                : "memory"
               );
    return res;
}

int main(){

	printf("\n%d\n%d\n", mpow(2, 4), pow(2, 4));

	return 0;
}
