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
            : "memory" , "r0", "r1", "r2", "r4"\
            );
               /*"arsm | fmuls\fmuld"*/
#define asmPowf(num,radix)\
  int one = 1;\
  int zero = 0;\
  asm volatile (\
            "vmov %0, %4\n"\
            "vcmp.f32 %2, %4\n\t"\
            "vmrs APSR_nzcv, FPSCR\n\t"\
            "blo lowpow%=\n\t"\
            "beq endpow%=\n"\
          "begpow%=:\n\t"\
            "vcmp.f32 %2, %4\n\t"\
            "vmrs APSR_nzcv, FPSCR\n\t"\
            "beq endpow%=\n\t"\
            "fmuls %0, %1, %0\n\t"\
            "vsub.f32 %2, %2, %3\n\t"\
            "b begpow%=\n"\
          "lowpow%=:\n\t"\
            "vmov %0, %4\n"\
          "endpow%=:\n\t"\
            "vmov %0, %0\n"\
            : "=t" (num)\
            : "t" (num), "t" (radix), "t"(one), "t"(zero), "0" (num)\
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
                : "memory", "r0", "r1", "r2", "r4"
               );
    return ret;
}

int main(){

int b = 4,i, radix = 10;
double t , t1, t2, t3[10000],t4[10000];
	double st, st1, st2,st3,st4;
int a;
	for (i = 0; i < 10000; i++) {

        t = wtime();
	a = asmpowi(b,radix);
        t = wtime() - t;st +=t;
	}
        st = st / 10000;

	for (i = 0; i < 10000; i++) {
        t1 = wtime();
				 b= 4; radix=10;
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

	printf("\nasmf:%d | time = %.16lf\nasmd:%d | time = %.16lf\nlm:%d | time = %.16lf\n", a,st, b,st1, c,st2);
        double r = 6.125;
	double res = 0, resasm = 0;

	for (i = 0; i < 10000; i++) {
        t3[i] = wtime();
        res = pow(r,radix);
        t3[i] = wtime() - t3[i];st3 +=t3[i];
	}
        st3 = st3 / 10000;
	for (i = 0; i < 10000; i++) {
        t4[i] = wtime();
				r =6.125;
	    	asmPowf(r,radix);
        t4[i] = wtime() - t4;st4 +=t4;
	}
resasm = r;
        st4 = st4 / 10000;
double	eps = res - resasm;
        eps = eps < 0 ? -eps : eps;
	printf("\nlm:%.16lf | time = %.16lf\nasmd:%.16lf | time = %.16lf \n -- %.16lf\n", res, st3, resasm,st4,eps);
  double disp_asm, disp_lib;
  for (int i = 0; i < 10000; ++i)
  {
  disp_asm += pow(t4[i] - st4, 2);
  disp_lib += pow(t3[i] - st3, 2);
  }
  disp_asm /= 10000;
  disp_lib /= 10000;
  printf("Masm: %.6f.\nMlib: %.6f.\nDasm: %.6f.\nDlib: %.6f.\nSasm: %.6f.\nSasm: %.6f.\n",
st4, st3, disp_asm, disp_lib, sqrt(disp_asm), sqrt(disp_lib));
	return 0;
}
