#include "../include/functions.h"
int threadnum = 7;
int threadn;
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

  one = (int32_t*)malloc(sizeof(int32_t) * realSize * realSize);
  two = (int32_t*)malloc(sizeof(int32_t) * realSize * realSize);
  rezult = (int32_t*)malloc(sizeof(int32_t) * realSize * realSize);

  for (i = 0; i < realSize; i++) {
    for (j = 0; j < realSize; j++) {
      if (i >= size) {
        one[i * size + j] = 0;
      } else if ( j >= size) {
        one[i * size + j] = 0;
      } else {
        one[i * size + j] = getrand(min,max);
      }
    }
  }

  for (i = 0; i < realSize; i++) {
    for (j = 0; j < realSize; j++) {
      if (i >= size) {
        two[i * size + j] = 0;
      } else if ( j >= size) {
        two[i * size + j] = 0;
      } else {
        two[i * size + j] = getrand(min,max);
      }
    }
  }

      printf("%d\n",realSize);
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
  threadnum = argc > 2 ? atoi(argv[2]) : 4;
  dat datat = {rezult,one, two,realSize,realSize};
  time = wtime();
   simpleMatrixProizvCacheObliviousp(&datat);
//  simpleMatrixProizvCacheOblivious(rezult, one, two, realSize, realSize);
  time = wtime() - time;
  
  while(threadn > 0);
  
  int flg = argc > 3 ? atoi(argv[3]) : 0;

  if (flg > 0){
  for (i = 0; i < realSize; i++) {
    for (j = 0; j < realSize; j++) {
      printf("%d\t", one[i * size + j]);
    }
      printf("\n");
  }
      printf("\n");
  
  for (i = 0; i < realSize; i++) {
    for (j = 0; j < realSize; j++) {
      printf("%d\t", two[i * size + j]);
    }
      printf("\n");
  }
      printf("\n");
  for (i = 0; i < realSize; i++) {
    for (j = 0; j < realSize; j++) {
      printf("%d\t", rezult[i * size + j]);
    }
      printf("\n");
  }}
      printf("\n");
  printf("simpleMatrixProizvCacheOblivious\t%.6lf \n" , time);


  return 0;
}
