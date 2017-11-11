#include <stdio.h>
#include <math.h>
#include <sys/time.h>

#define asmPow(num,radix) asm volatile (\
	        "mov r0, #1\n\t"\
	        "mov r1, %1\n\t"\
	        "mov r2, %2\n\t"\
	        "mov r4, #0\n\t"\
	        "cmp r2, r4\n\t"\
                "blo lowpow%=\n\t"\
                "beq endpow%=\n"\
              "begpow%=:\n\t"\
                "cmp r2, r4\n\t"\
                "beq endpow%=\n\t"\
                "mul r0, r0, r1\n\t"\
                "sub r2, r2, #1\n\t"\
                "b begpow%=\n"\
              "lowpow%=:\n\t"\
                "mov r0, #0\n"\
              "endpow%=:\n\t"\
                "mov %0, r0\n"\
                : "=r" (num)\
                : "r" (num), "r" (radix)\
                : "memory" \
               );

double wtime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1E-6;
}

volatile int asmpow(int num, int radix)
{
  int ret = -1;
  asm volatile (
               // "arsm | fmuls\fmuld"
	        "mov r0, #1\n\t"
	        "mov r1, %1\n\t"
	        "mov r2, %2\n\t"
	        "mov r4, #0\n\t"
	        "cmp r2, r4\n\t"
                "blo lowpow%=\n\t"
                "beq endpow%=\n"
              "begpow%=:\n\t"
                "cmp r2, r4\n\t"
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
    return ret;
}

int main(){

int b = 4;

        double t = wtime();
        int a = asmpow(4,10);
        t = wtime() - t;

        double t1 = wtime();
        asmPow(b,10);
        t1 = wtime() - t1;

        double t2 = wtime();
        int c = pow(4,10);
        t2 = wtime() - t2;

	printf("\n%d | time = %.16lf\n%d | time = %.16lf\n%d | time = %.16lf\n", a,t, b,t1, c,t2);

	return 0;
}
