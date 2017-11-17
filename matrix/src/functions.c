#include "../include/functions.h"
#include <pthread.h>
#define TRESHOLD 511
extern int threadnum;
extern int threadn;

extern int print;
int mutlock = 0;

pthread_mutex_t incmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t compmut = PTHREAD_MUTEX_INITIALIZER;

int myPow(int a, int b)
{
  int tmp = a;
  if (b == 0) {
    return 1;
  } else if (b == 1) {
    return a;
  } else {
    int i = 1;

    for (i = 1; i < b; i++)
      a = a * tmp;
    return a;
  }
}
/*---------------------------------------------------------------------------*/
double wtime()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double)t.tv_sec + (double)t.tv_usec * 1E-6;
}
/*---------------------------------------------------------------------------*/
void simpleMatrixProizvAsm(int32_t** first, int32_t** second,
                           int32_t** rezult, int size){
  int i,j,k;
  for (i = 0; i < size; i++) {
    for (k = 0; k < size; k++) {
      for (j = 0; j < size; j++) {
        asm volatile (
                      ".intel_syntax noprefix\n\t"
                      "mul edx\n\t"
                      "add %0, eax\n\t"
                      : "=m" (rezult[i][j])
                      : "a" (first[i][k]), "d"(second[k][j])
                      :
                      );
      }
    }
  }
}
/*---------------------------------------------------------------------------*/
void simpleMatrixProizv(int32_t** first, int32_t** second,
                        int32_t** rezult, int size){
  int i,j,k;
  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      for (k = 0; k < size; k++) {
        rezult[i][j] += first[i][k] * second[k][j];
      }
    }
  }
}
/*---------------------------------------------------------------------------*/
void simpleMatrixProizvCache(int32_t** first, int32_t** second,
                             int32_t** rezult, int size){
  int i,j,k;
  for (i = 0; i < size; i++) {
    for (k = 0; k < size; k++) {
      for (j = 0; j < size; j++) {
        rezult[i][j] += first[i][k] * second[k][j];
      }
    }
  }
}
/*---------------------------------------------------------------------------*/
void simpleMatrixProizvCacheOblivious(int32_t *C,  int32_t *A,  int32_t *B,
                                      int size, int rowsize)
{
  if (size == 2)
  {
    const int ind11 = 0;
    const int ind12 = 1;
    const int ind21 = rowsize;
    const int ind22 = rowsize + 1;

    C[ind11] += A[ind11] * B[ind11] + A[ind12] * B[ind21];
    C[ind12] += A[ind11] * B[ind12] + A[ind12] * B[ind22];
    C[ind21] += A[ind21] * B[ind11] + A[ind22] * B[ind21];
    C[ind22] += A[ind21] * B[ind12] + A[ind22] * B[ind22];
  } else {
    const int ind11 = 0;
    const int ind12 = size / 2;
    const int ind21 = (size / 2) * rowsize;
    const int ind22 = (size / 2) * (rowsize + 1);

    // C11 += A11 * B11
    simpleMatrixProizvCacheOblivious(C + ind11, A + ind11, B + ind11, size / 2, rowsize);
    // C11 += A12 * B21
    simpleMatrixProizvCacheOblivious(C + ind11, A + ind12, B + ind21, size / 2, rowsize);

    // C12 += A11 * B12
    simpleMatrixProizvCacheOblivious(C + ind12, A + ind11, B + ind12, size / 2, rowsize);
    // C12 += A12 * B22
    simpleMatrixProizvCacheOblivious(C + ind12, A + ind12, B + ind22, size / 2, rowsize);

    // C21 += A21 * B11
    simpleMatrixProizvCacheOblivious(C + ind21, A + ind21, B + ind11, size / 2, rowsize);
    // C21 += A22 * B21
    simpleMatrixProizvCacheOblivious(C + ind21, A + ind22, B + ind21, size / 2, rowsize);

    // C22 += A21 * B12
    simpleMatrixProizvCacheOblivious(C + ind22, A + ind21, B + ind12, size / 2, rowsize);
    // C22 += A22 * B22
    simpleMatrixProizvCacheOblivious(C + ind22, A + ind22, B + ind22, size / 2, rowsize);
  }
}
/*----------------------------------------------------------------------------*/
void * simpleMatrixProizvCacheObliviousp(void* ptr)
{
  dat * p = (dat *)(ptr);
  //printf("currentlevel = %d!\n",  p->cursorlevel);
  //  printf("%d\n", p->size); 
  
  if (p->size == 2)
  {
    const int ind11 = 0;
    const int ind12 = 1;
    const int ind21 = p->rowsize;
    const int ind22 = p->rowsize + 1;
 pthread_mutex_lock(&compmut);
    p->C[ind11] += p->A[ind11] * p->B[ind11] + p->A[ind12] * p->B[ind21];
    p->C[ind12] += p->A[ind11] * p->B[ind12] + p->A[ind12] * p->B[ind22];
    p->C[ind21] += p->A[ind21] * p->B[ind11] + p->A[ind22] * p->B[ind21];
    p->C[ind22] += p->A[ind21] * p->B[ind12] + p->A[ind22] * p->B[ind22];
    pthread_mutex_unlock(&compmut);
  } else {
    int tsize = p->size/2;
    const int ind11 = 0;
    const int ind12 = tsize;
    const int ind21 = tsize * p->rowsize;
    const int ind22 = tsize * (p->rowsize + 1);
    pthread_t tid[8];
    dat argum[8] = {
{p->C + ind11, p->A + ind11, p->B + ind11, tsize, p->rowsize, p->cursorlevel + 1, p->needlevel,0},//
{p->C + ind11, p->A + ind12, p->B + ind21, tsize, p->rowsize, p->cursorlevel + 1, p->needlevel,0},//
{p->C + ind12, p->A + ind11, p->B + ind12, tsize, p->rowsize, p->cursorlevel + 1, p->needlevel,0},//
{p->C + ind12, p->A + ind12, p->B + ind22, tsize, p->rowsize, p->cursorlevel + 1, p->needlevel,0},//
{p->C + ind21, p->A + ind21, p->B + ind11, tsize, p->rowsize, p->cursorlevel + 1, p->needlevel,0},//
{p->C + ind21, p->A + ind22, p->B + ind21, tsize, p->rowsize, p->cursorlevel + 1, p->needlevel,0},
{p->C + ind22, p->A + ind21, p->B + ind12, tsize, p->rowsize, p->cursorlevel + 1, p->needlevel,0},
{p->C + ind22, p->A + ind22, p->B + ind22, tsize, p->rowsize, p->cursorlevel + 1, p->needlevel,0}
};

    // C11 += A11 * B11
    if (threadnum > 0 && p->size > TRESHOLD){
      
    pthread_mutex_lock(&incmutex);
    if (threadn < threadnum && p->needlevel == p->cursorlevel) {
    threadn++;
    pthread_mutex_unlock(&incmutex);
      if(print) 
      printf("thread %d is %d! level = %d\n", threadn - 1, threadnum, p->cursorlevel);
      argum[0].isthread = 1;
    pthread_create(&tid[0],NULL,simpleMatrixProizvCacheObliviousp, &argum[0]);
    } else {
      pthread_mutex_unlock(&incmutex);
      simpleMatrixProizvCacheObliviousp(&argum[0]);
    }
      
    } else 
    simpleMatrixProizvCacheObliviousp(&argum[0]);
    
    // C11 += A12 * B21
    if (threadnum > 0 && p->size > TRESHOLD){
      
    pthread_mutex_lock(&incmutex);
    if (threadn < threadnum && p->needlevel == p->cursorlevel) {
    threadn++;
    pthread_mutex_unlock(&incmutex);
      if(print) 
      printf("thread %d is %d! level = %d\n", threadn - 1, threadnum, p->cursorlevel);
      argum[1].isthread = 1;
    pthread_create(&tid[1],NULL,simpleMatrixProizvCacheObliviousp, &argum[1]);
    } else {
    pthread_mutex_unlock(&incmutex);
      simpleMatrixProizvCacheObliviousp(&argum[1]);
    }
    
    } else 
    simpleMatrixProizvCacheObliviousp(&argum[1]);
      
    // C12 += A11 * B12
    if (threadnum > 0 && p->size > TRESHOLD){
      
    pthread_mutex_lock(&incmutex);
    if (threadn < threadnum && p->needlevel == p->cursorlevel) {
    threadn++;
    pthread_mutex_unlock(&incmutex);
      if(print) 
      printf("thread %d is %d! level = %d\n", threadn - 1, threadnum, p->cursorlevel);
      argum[2].isthread = 1;
    pthread_create(&tid[2],NULL,simpleMatrixProizvCacheObliviousp, &argum[2]);
    } else {
    pthread_mutex_unlock(&incmutex);
      simpleMatrixProizvCacheObliviousp(&argum[2]);
    }
    } else 
    simpleMatrixProizvCacheObliviousp(&argum[2]);
      
    // C12 += A12 * B22
    if (threadnum > 0 && p->size > TRESHOLD){
      
    pthread_mutex_lock(&incmutex);
    if (threadn < threadnum && p->needlevel == p->cursorlevel) {
    threadn++;
    pthread_mutex_unlock(&incmutex);
      if(print) 
      printf("thread %d is %d! level = %d\n", threadn - 1, threadnum, p->cursorlevel);
      argum[3].isthread = 1;
    pthread_create(&tid[3],NULL,simpleMatrixProizvCacheObliviousp, &argum[3]);
    } else {
    pthread_mutex_unlock(&incmutex);
      simpleMatrixProizvCacheObliviousp(&argum[3]);
    }
    
    } else 
    simpleMatrixProizvCacheObliviousp(&argum[3]);

      
    // C21 += A21 * B11
    if (threadnum > 0 && p->size > TRESHOLD){
      
    pthread_mutex_lock(&incmutex);
    if (threadn < threadnum && p->needlevel == p->cursorlevel) {
    threadn++;
    pthread_mutex_unlock(&incmutex);
      if(print) 
      printf("thread %d is %d! level = %d\n", threadn - 1, threadnum, p->cursorlevel);
      argum[4].isthread = 1;
    pthread_create(&tid[4],NULL,simpleMatrixProizvCacheObliviousp, &argum[4]);
    } else {
    pthread_mutex_unlock(&incmutex);
      simpleMatrixProizvCacheObliviousp(&argum[4]);
    }
    
    } else 
    simpleMatrixProizvCacheObliviousp(&argum[4]);
    
    // C21 += A22 * B21
    if (threadnum > 0 && p->size > TRESHOLD){
      
    pthread_mutex_lock(&incmutex);
    if (threadn < threadnum && p->needlevel == p->cursorlevel) {
    threadn++;
    pthread_mutex_unlock(&incmutex);
      if(print) 
      printf("thread %d is %d! level = %d\n", threadn - 1, threadnum, p->cursorlevel);
      argum[5].isthread = 1;
    pthread_create(&tid[5],NULL,simpleMatrixProizvCacheObliviousp, &argum[5]);
    } else {
    pthread_mutex_unlock(&incmutex);
      simpleMatrixProizvCacheObliviousp(&argum[5]);
    }
    
    } else 
    simpleMatrixProizvCacheObliviousp(&argum[5]);
    
    // C22 += A21 * B12
    if (threadnum > 0 && p->size > TRESHOLD){
      
    pthread_mutex_lock(&incmutex);
    if (threadn < threadnum && p->needlevel == p->cursorlevel) {
    threadn++;
    pthread_mutex_unlock(&incmutex);
      if(print) 
      printf("thread %d is %d! level = %d\n", threadn - 1, threadnum, p->cursorlevel);
      argum[6].isthread = 1;
    pthread_create(&tid[6],NULL,simpleMatrixProizvCacheObliviousp, &argum[6]);
    } else {
    pthread_mutex_unlock(&incmutex);
      simpleMatrixProizvCacheObliviousp(&argum[6]);
    }
    
    } else 
    simpleMatrixProizvCacheObliviousp(&argum[6]);

    // C22 += A22 * B22
    if (threadnum > 0 && p->size > TRESHOLD){
      
    pthread_mutex_lock(&incmutex);
    if (threadn < threadnum && p->needlevel == p->cursorlevel) {
    threadn++;
    pthread_mutex_unlock(&incmutex);
      if(print) 
      printf("(7)thread %d is %d! level = %d\n", threadn - 1, threadnum, p->cursorlevel);
      argum[7].isthread = 1;
    pthread_create(&tid[7],NULL,simpleMatrixProizvCacheObliviousp, &argum[7]);
      pthread_join(tid[7], NULL);
      if(print) 
      printf("(7)thread %d is %d join success! level = %d\n", threadn - 1, threadnum, p->cursorlevel);
    } else {
      pthread_mutex_unlock(&incmutex);
      simpleMatrixProizvCacheObliviousp(&argum[7]);
    }
    
    } else 
    simpleMatrixProizvCacheObliviousp(&argum[7]);
 // }
//  p->thr[0]--;
  }
  if (p->cursorlevel == p->needlevel + 1 && p->isthread == 1) {
    pthread_mutex_lock(&incmutex);
    threadn--;
      pthread_mutex_unlock(&incmutex);
      printf("thread exit %d %d %d\n", threadn, threadnum, p->cursorlevel);
  }
  if (p->cursorlevel == 1){
    while (threadn > 0){ printf("do nothing\n"); sleep(2);}
  }
  return NULL;
}
