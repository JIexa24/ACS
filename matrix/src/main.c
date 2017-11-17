#include "../include/functions.h"
int threadnum = 7;
int threadn;
int print;
int cntr = 0;
extern pthread_spinlock_t complock;
int getrand(int32_t min, int32_t max)
{
  return (double)rand() / (RAND_MAX + 1.0) * (max - min) + min;
}
/*---------------------------------------------------------------------------*/
int main(int argc, char** argv)
{
  assert(!(argc < 2));

  int i                  = 0;
  int j                  = 0;
  int32_t** matrixOne    = NULL;
  int32_t** matrixTwo    = NULL;
  int32_t** matrixRezult = NULL;
  int32_t* one           = NULL;
  int32_t* two           = NULL;
  int32_t* rezult        = NULL;
  int size               = atoi(argv[1]);
  int realSize           = 2;
  int32_t min            = -5;
  int32_t max            = 5;
  int needlevel = 0;

  matrixOne = (int32_t**)malloc(sizeof(int32_t*) * realSize);
  for (i = 0; i < realSize; i++) {
    matrixOne[i] = (int32_t*)malloc(sizeof(int32_t) * realSize);
  }

  matrixTwo = (int32_t**)malloc(sizeof(int32_t*) * realSize);
  for (i = 0; i < realSize; i++) {
    matrixTwo[i] = (int32_t*)malloc(sizeof(int32_t) * realSize);
  }

  matrixRezult = (int32_t**)malloc(sizeof(int32_t*) * realSize);
  for (i = 0; i < realSize; i++) {
    matrixRezult[i] = (int32_t*)malloc(sizeof(int32_t) * realSize);
  }

  for (i = 0; i < realSize; i++) {
    for (j = 0; j < realSize; j++) {
      if ((i >= size) | (j >= size)) {
        matrixOne[i][j] = 0;
        matrixTwo[i][j] = 0;
      } else {
        matrixOne[i][j] = getrand(min,max);
        matrixTwo[i][j] = getrand(min,max);
      }
      matrixRezult[i][j] = 0;
    }
  }

  double time = wtime();
 // simpleMatrixProizv(matrixOne, matrixTwo, matrixRezult, size);
  time = wtime() - time;
  printf("simpleMatrixProizv\t%.6lf\n" , time);

  for (i = 0; i < realSize; i++) {
    for (j = 0; j < realSize; j++) {
      matrixRezult[i][j] = 0;
    }
  }

  time = wtime();
 // simpleMatrixProizvAsm(matrixOne, matrixTwo, matrixRezult, size);
  time = wtime() - time;
  printf("simpleMatrixProizvAsm\t%.6lf\n" , time);

  for (i = 0; i < realSize; i++) {
    for (j = 0; j < realSize; j++) {
      matrixRezult[i][j] = 0;
    }
  }

  time = wtime();
 // simpleMatrixProizvCache(matrixOne, matrixTwo, matrixRezult, size);
  time = wtime() - time;

  printf("simpleMatrixProizvCache\t%.6lf\n" , time);

  for(i = 0; i < realSize; i++){
    free(matrixOne[i]);
    free(matrixTwo[i]);
  }
  free(matrixOne);
  free(matrixTwo);

  for (i = 0; i < realSize; i++) {
    free(matrixRezult[i]);
  }
  free(matrixRezult);
  if (size > 1)
  for (i = 0; ; i++) {
    if ((size > myPow(2,i)) & (size <= myPow(2, i + 1))) {
      realSize = myPow(2, i + 1);
      break;
    }
  }

  one = (int32_t*)calloc(realSize * realSize, sizeof(int32_t));
  two = (int32_t*)calloc(realSize * realSize, sizeof(int32_t));
  rezult = (int32_t*)calloc(realSize * realSize, sizeof(int32_t));
  printf("%d %d\n",size, realSize);
  for (i = 0; i < realSize; i++) {
    for (j = 0; j < realSize; j++) {
      if (i < size && j < size) {
         one[i * realSize + j] = j+1;
      } else {
         one[i * realSize + j] = 0;
      }
    }
  }

  for (i = 0; i < realSize; i++) {
    for (j = 0; j < realSize; j++) {
      if (i < size && j < size) {
          two[i * realSize + j] = i+1;
      } else {
         two[i * realSize + j] = 0;
      }
    }
  }

   //   printf("%d\n",realSize);
  for (i = 0; i < realSize; i++) {
    for (j = 0; j < realSize; j++) {
      rezult[i * size + j] = 0;
    }
  }

/*  for (i = 0; i < realSize; i++) {
    for (j = 0; j < realSize; j++) {
      printf("%d(%d %d|%d %d)\t", one[i * size + j], i , j, size, realSize);
    }
      printf("\n");
  }*/
      printf("\n");
  threadnum = 0;
  threadn = 0;
  threadnum = argc > 2 ? atoi(argv[2]) -  1 : 0;

  printf("threadn %d\n",threadn);
  printf("threadnum %d\n",threadnum);
  if (threadnum <= 8) needlevel = 1;
  for (i = 0; ; i++){
    if (threadnum <= (myPow(8,i + 1)) && threadnum > (myPow(8,i))) {
     needlevel = i + 1;
      break;
    }
    if (threadnum == 0) {
     needlevel = 0;break; 
    }
  }
  int flg = argc > 3 ? atoi(argv[3]) : 0;
  print = flg;  
  printf("needlvl %d\n",needlevel);
  dat datat = {rezult,one, two,realSize,realSize, 1, needlevel, 0};
  pthread_spin_init(&complock, PTHREAD_PROCESS_PRIVATE);
  time = wtime();
   simpleMatrixProizvCacheObliviousp(&datat);
//  simpleMatrixProizvCacheOblivious(rezult, one, two, realSize, realSize);
  time = wtime() - time;
  pthread_spin_destroy(&complock);

  
//  sleep(2);
//  int flg = argc > 3 ? atoi(argv[3]) : 0;
  //print = flg;
  if (flg > 1){
  for (i = 0; i < realSize; i++) {
    for (j = 0; j < realSize; j++) {
      printf("%d\t", one[i * realSize + j]);
    }
      printf("\n");
  }
      printf("\n");
  
  for (i = 0; i < realSize; i++) {
    for (j = 0; j < realSize; j++) {
      printf("%d\t", two[i * realSize + j]);
    }
      printf("\n");
  }
      printf("\n");
  for (i = 0; i < realSize; i++) {
    for (j = 0; j < realSize; j++) {
      printf("%d\t", rezult[i * realSize + j]);
    }
      printf("\n");
  }
  }
      printf("\n");
  printf("simpleMatrixProizvCacheOblivious\t%.6lf \n\n" , time - cntr * 2);
  int sum = 0;
  for (i = 1; i <= size; i++) {
    sum += myPow(i,2);
  }
  printf("%d ==%d\n",rezult[(size - 1)*(size-1)], sum);
  
    free(two);
    free(one);
    free(rezult);
  return 0;
}
