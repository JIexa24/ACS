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
  assert(!(argc < 5));

  int i                  = 0;
  int j                  = 0;
  int32_t** matrixOne    = NULL;
  int32_t** matrixTwo    = NULL;
  int32_t** matrixRezult = NULL;
  int32_t* one           = NULL;
  int32_t* two           = NULL;
  int32_t* rezult        = NULL;
  int size               = atoi(argv[1]);
  int realSize           = size;
  int32_t min            = -5;
  int32_t max            = 5;
  int needlevel = 0;
  int sizei = size, sizek, sizej;
  double time;
  threadnum = argc > 2 ? atoi(argv[2]) : 1;
  
  sizej = atoi(argv[4]);
  sizek = atoi(argv[3]);
  
  matrixOne = (int32_t**)malloc(sizeof(int32_t*) * sizei);
  for (i = 0; i < realSize; i++) {
    matrixOne[i] = (int32_t*)malloc(sizeof(int32_t) * sizek);
  }

  matrixTwo = (int32_t**)malloc(sizeof(int32_t*) * sizek);
  for (i = 0; i < realSize; i++) {
    matrixTwo[i] = (int32_t*)malloc(sizeof(int32_t) * sizej);
  }

  matrixRezult = (int32_t**)malloc(sizeof(int32_t*) * sizei);
  for (i = 0; i < realSize; i++) {
    matrixRezult[i] = (int32_t*)malloc(sizeof(int32_t) * sizej);
  }

  for (i = 0; i < sizei; i++) {
    for (j = 0; j < sizek; j++) {
        matrixOne[i][j] = j+1;
    }
  }
  for (i = 0; i < sizek; i++) {
    for (j = 0; j < sizej; j++) {
        matrixTwo[i][j] = i+1;
    }
  }
  for (i = 0; i < sizei; i++) {
    for (j = 0; j < sizej; j++) {
        matrixRezult[i][j] = 0;
    }
  }

  int step = (int)((double)sizei/(double)threadnum);int position = 0;
  pthread_t *tid = malloc(threadnum * sizeof(pthread_t));
  pdat *td = malloc(threadnum * sizeof(pdat));
  
  time = wtime();
  for(i = 0; i < threadnum; i++) {
    td[i].A = matrixOne;
    td[i].B = matrixTwo;
    td[i].C = matrixRezult;
    td[i].sizej = realSize;
    td[i].sizek = realSize;
    td[i].starti = position;
    position+=step;
    td[i].sizei = (i == threadnum - 1) ? sizei : position;
    pthread_create(&tid[i],NULL,simpleMatrixProizvp, &td[i]);
  }
  for(i = 0; i < threadnum; i++) {
  pthread_join(tid[i],NULL);
  }
  time = wtime() - time;
  printf("simpleMatrixProizvp\t%.6lf\n" , time);
  int sum = 0;
  for (i = 1; i <= size; i++) {
    sum += myPow(i,2);
  }
  printf("%d == %d\n",matrixRezult[(size - 1)][(size-1)], sum);

  for (i = 0; i < realSize; i++) {
    for (j = 0; j < realSize; j++) {
      matrixRezult[i][j] = 0;
    }
  }

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
  int flg = 0;
  print = flg;  
  printf("needlvl %d\n",needlevel);
  
  dat datat = {rezult,one, two,realSize,realSize, 1, needlevel, 0};
  pthread_spin_init(&complock, PTHREAD_PROCESS_PRIVATE);
  time = wtime();
 //  simpleMatrixProizvCacheObliviousp(&datat);
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
  printf("simpleMatrixProizvCacheOblivious\t%.6lf \n\n" , time - (cntr > 0 ? 2 : 0));// - cntr * 2);
   sum = 0;
  for (i = 1; i <= size; i++) {
    sum += myPow(i,2);
  }
  //printf("%d == %d\n",rezult[(size - 1)*(size-1)], sum);
  
    free(two);
    free(one);
    free(rezult);
  return 0;
}
