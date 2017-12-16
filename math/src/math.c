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
	        "vcmp.f64 d2, d4\n\t"\
            "vmrs APSR_nzcv, FPSCR\n\t"\
            "blo lowpow%=\n\t"\
          "beq endpow%=\n"\
            "begpow%=:\n\t"\
            "vcmp.f64 d2, d4\n\t"\
            "vmrs APSR_nzcv, FPSCR\n\t"\
            "beq endpow%=\n\t"\
            "fmuld d0, d0, d1\n\t"\
            "vsub d2, d2, #1\n\t"\
            "b begpow%=\n"\
          "lowpow%=:\n\t"\
            "vmov d0, #0\n"\
          "endpow%=:\n\t"\
            "vmov %0, d0\n"\
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
            "vcmp.f32 s2, s4\n\t"\
            "vmrs APSR_nzcv, FPSCR\n\t"\
            "blo lowpow%=\n\t"\
            "beq endpow%=\n"\
          "begpow%=:\n\t"\
            "vcmp.f32 s2, s4\n\t"\
            "vmrs APSR_nzcv, FPSCR\n\t"\
            "beq endpow%=\n\t"\
            "fmuls s0, s0, s1\n\t"\
            "vsub s2, s2, #1\n\t"\
            "b begpow%=\n"\
          "lowpow%=:\n\t"\
            "vmov s0, #0\n"\
          "endpow%=:\n\t"\
            "vmov %0, s0\n"\
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
/*
volatile double asmpowlf(double num, int radix)
{
  int ret = -1;
  asm volatile (
               // "arsm | fmuls\fmuld"
	          "vmov d0, #1\n\t"
	          "vmov d1, %1\n\t"
	          "vmov d2, %2\n\t"
	          "vmov d4, #0\n\t"
	          "vcmp.f64 d2, d4\n\t"
              "vmrs APSR_nzcv, FPSCR\n\t"
              "blo lowpow%=\n\t"
              "beq endpow%=\n"
            "begpow%=:\n\t"
              "vcmp.f64 d2, d4\n\t"
              "vmrs APSR_nzcv, FPSCR\n\t"
              "beq endpow%=\n\t"
              "fmuld d0, d0, d1\n\t"
              "vsub d2, d2, #1\n\t"
              "b begpow%=\n"
            "lowpow%=:\n\t"
              "vmov d0, #0\n"
            "endpow%=:\n\t"
              "vmov %0, d0\n"
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
	            "vcmp.f32 s2, s4\n\t"
                "vmrs APSR_nzcv, FPSCR\n\t"
                "blo lowpow%=\n\t"
                "beq endpow%=\n"
              "begpow%=:\n\t"
                "vcmp.f32 s2, s4\n\t"
                "vmrs APSR_nzcv, FPSCR\n\t"
                "beq endpow%=\n\t"
                "fmuls s0, s0, s1\n\t"
                "vsub s2, s2, #1\n\t"
                "b begpow%=\n"
              "lowpow%=:\n\t"
                "vmov s0, #0\n"
              "endpow%=:\n\t"
                "vmov %0, s0\n"
                : "=r" (ret)
                : "r" (num), "r" (radix)
                : "memory"
               );
    return ret;
}
*/
//#define asmPow(X, Y) _Generic((X), default: asmPowlf, double: asmPowlf, int: asmPowi, \
 //                             float: asmPowf)
//#define asmpow(X, Y) _Generic((X), default: asmpowlf, double: asmpowlf, int: asmpowi, \
 //                             float: asmpowf)

int main(){

int b = 4,i, radix = 10;
double t , t1, t2, t3,t4;
	double st, st1, st2,st3,st4;
int a;
	for (i = 0; i < 10000; i++) {

        t = wtime();
	a = asmpowi(b,radix);
        t = wtime() - t;st +=t;
	}
        st = st / 10000;
	b = 4;
	for (i = 0; i < 10000; i++) {
        t1 = wtime();
        asmPowi(b,radix);
        t1 = wtime() - t1;st1 +=t1;
	}
        st1 = st1 / 10000;
int c;
	for (i = 0; i < 10000; i++) {
        t2 = wtime();
         c = pow(4,radix);
        t2 = wtime() - t2;st2 +=t2;
	}
        st2 = st2 / 10000;

	printf("\n%d | time = %.16lf\n%d | time = %.16lf\n%d | time = %.16lf\n", a,st, b,st1, c,st2);
        double r = 6.125;
	double res = 0, resasm = 0;

	for (i = 0; i < 10000; i++) {
        t3 = wtime();
        res = pow(r,radix);
        t3 = wtime() - t3;st3 +=t3;
	}
        st3 = st3 / 10000;
	for (i = 0; i < 10000; i++) {
        t4 = wtime();
	//    	resasm = asmpowlf(r,radix);
        t4 = wtime() - t4;st4 +=t4;
	}
        st4 = st4 / 10000;
double	eps = res - resasm;
        eps = eps < 0 ? -eps : eps;
	printf("\n%.16lf | time = %.16lf\n%.16lf | time = %.16lf \n -- %.16lf\n", res, st3, resasm,st4,eps);
	return 0;
}
