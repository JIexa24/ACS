#include <stdio.h>
#include <math.h>
#include <sys/time.h>

#define asmPowi(num,radix) asm volatile (\
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

               /* "arsm | fmuls\fmuld"*/
#define asmPowlf(num,radix) asm volatile (\
	        "vmov d0, #1\n\t"\
	        "vmov d1, %1\n\t"\
	        "vmov d2, %2\n\t"\
	        "vmov d4, #0\n\t"\
	        "cmp d2, d4\n\t"\
                "blo lowpow%=\n\t"\
                "beq endpow%=\n"\
              "begpow%=:\n\t"\
                "cmp d2, d4\n\t"\
                "beq endpow%=\n\t"\
                "fmuld d0, d0, d1\n\t"\
                "sub d2, d2, #1\n\t"\
                "b begpow%=\n"\
              "lowpow%=:\n\t"\
                "mov d0, #0\n"\
              "endpow%=:\n\t"\
                "mov %0, d0\n"\
                : "=r" (num)\
                : "r" (num), "r" (radix)\
                : "memory"\
               );
               /*"arsm | fmuls\fmuld"*/
#define asmPowf(num,radix) asm volatile (\
	        "vmov s0, #1\n\t"\
	        "vmov s1, %1\n\t"\
	        "vmov s2, %2\n\t"\
	        "vmov s4, #0\n\t"\
	        "cmp s2, s4\n\t"\
                "blo lowpow%=\n\t"\
                "beq endpow%=\n"\
              "begpow%=:\n\t"\
                "cmp s2, s4\n\t"\
                "beq endpow%=\n\t"\
                "fmuls s0, s0, s1\n\t"\
                "sub s2, s2, #1\n\t"\
                "b begpow%=\n"\
              "lowpow%=:\n\t"\
                "mov s0, #0\n"\
              "endpow%=:\n\t"\
                "mov %0, s0\n"\
                : "=r" (num)\
                : "r" (num), "r" (radix)\
                : "memory"\
               );

double wtime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1E-6;
}

volatile int asmpowi(int num, int radix)
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

volatile double asmpowlf(double num, int radix)
{
  int ret = -1;
  asm volatile (
               // "arsm | fmuls\fmuld"
	        "vmov d0, #1\n\t"
	        "vmov d1, %1\n\t"
	        "vmov d2, %2\n\t"
	        "vmov d4, #0\n\t"
	        "cmp d2, d4\n\t"
                "blo lowpow%=\n\t"
                "beq endpow%=\n"
              "begpow%=:\n\t"
                "cmp d2, d4\n\t"
                "beq endpow%=\n\t"
                "fmuld d0, d0, d1\n\t"
                "sub d2, d2, #1\n\t"
                "b begpow%=\n"
              "lowpow%=:\n\t"
                "mov d0, #0\n"
              "endpow%=:\n\t"
                "mov %0, d0\n"
                : "=r" (ret)
                : "r" (num), "r" (radix)
                : "memory"
               );
    return ret;
}

volatile float asmpowf(float num, int radix)
{
  int ret = -1;
  asm volatile (
               // "arsm | fmuls\fmuld"
	        "vmov s0, #1\n\t"
	        "vmov s1, %1\n\t"
	        "vmov s2, %2\n\t"
	        "vmov s4, #0\n\t"
	        "cmp s2, s4\n\t"
                "blo lowpow%=\n\t"
                "beq endpow%=\n"
              "begpow%=:\n\t"
                "cmp s2, s4\n\t"
                "beq endpow%=\n\t"
                "fmuls s0, s0, s1\n\t"
                "sub s2, s2, #1\n\t"
                "b begpow%=\n"
              "lowpow%=:\n\t"
                "mov s0, #0\n"
              "endpow%=:\n\t"
                "mov %0, s0\n"
                : "=r" (ret)
                : "r" (num), "r" (radix)
                : "memory"
               );
    return ret;
}

//#define asmPow(X, Y) _Generic((X), default: asmPowlf, double: asmPowlf, int: asmPowi, \
 //                             float: asmPowf)
//#define asmpow(X, Y) _Generic((X), default: asmpowlf, double: asmpowlf, int: asmpowi, \
 //                             float: asmpowf)

int main(){

int b = 4;

        double t = wtime();
        int a = asmpowi(4,10);
        t = wtime() - t;

        double t1 = wtime();
        asmPowi(b,10);
        t1 = wtime() - t1;

        double t2 = wtime();
        int c = pow(4,10);
        t2 = wtime() - t2;

	printf("\n%d | time = %.16lf\n%d | time = %.16lf\n%d | time = %.16lf\n", a,t, b,t1, c,t2);

	return 0;
}
