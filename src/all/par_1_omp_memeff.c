/******************************************************************************
* FILE:
* DESCRIPTION:
* AUTHORS:
* LAST REVISED:
******************************************************************************/
#include <omp.h>
#include <stdio.h>
#include "rdtsc.h"
#include <stdlib.h>
#include <string.h>
// #include <omp.h>
#include <limits.h>
#include <math.h>


#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
static const int NUMBER_OF_THREADS = 4;
int** DMatrix;
int n, m, k;

char* pattern;
char* text;

void printMatrix() {
  // printf("\n ");
  for (int j = 0; j <= 2; j++) {
    for (int i = 0; i <= m; i++) {
      printf("|%d|", DMatrix[j][i]);
    }
    printf("\n");
  }

  printf("\n \n ");

}

void readTextandPattern(char *argv[]) {
  char *textFileName;
  char *patternFileName;
  textFileName = argv[1];
  patternFileName = argv[2];
  //reading from text file
  FILE *f = fopen(textFileName, "r");
  if (f == NULL)
  {
    perror("Error opening file");
    return ;
  }
  fseek(f, 0, SEEK_END);
  int SIZE = ftell(f);

  fseek(f, 0, SEEK_SET);

  char textBuf[SIZE + 1];
  if (fgets( textBuf, SIZE + 1, f) != NULL) {
    // printf("text read correctly\n");
    text = textBuf;
  } else {
    // printf("returned null \n");
  }
  fclose(f);

  f = fopen(patternFileName, "r");
  if (f == NULL)
  {
    perror("Error opening file");
    return;
  }
  fseek(f, 0, SEEK_END);
  SIZE = ftell(f);
  fseek(f, 0, SEEK_SET);
  char patternBuf[SIZE + 1];
  if (fgets( patternBuf, SIZE + 1, f) != NULL) {
    // printf("pattern read correctly\n");
    pattern = patternBuf;
  } else {
    // printf("returned null \n");
  }

  fclose(f);
}



int mod(int a, int b)
{
  int r = a % b;
  return r < 0 ? r + b : r;
}


int main(int argc, char *argv[]) {
	unsigned long long t1, t2;
	t1=rdtsc();
  int nthreads, tid;


  if ( argc != 4 ) /* argc should be 2 for correct execution */
  {

    printf( "It looks like your parameters are wrong" );
  }

  else {
    readTextandPattern(argv);
    m = strlen(pattern);
    n = strlen(text);
    k = atoi(argv[3]);
  }

  DMatrix = (int **) malloc(3 * sizeof(int *));

  for ( int i = 0; i < 3; i++) {

    DMatrix[i] = (int *) malloc((m + 1) * sizeof(int));
  }



  int j = 0;
  DMatrix[0][0] = 0;


  omp_set_num_threads(NUMBER_OF_THREADS);
  #pragma omp parallel
  {
    nthreads = omp_get_num_threads();

    int ID = omp_get_thread_num();
    int minVal = m;
    int j = 0;
    /*
    *   D MATRIX INITIALIZATION
    */


    for (int diag = 0; diag <= n + m - 1; diag++) {

      for (int i = ID; i <= m; i = i + nthreads) {
        // printf("d:%d\n", diag);
        // printf("i:%d\n", i);

        j = diag - i + 1;

        // printf("i is %d and j is %d \n", i, j);
        if (i == 0) {
          DMatrix[j % 3][i] = 0;
          // printf("set %d %d %d\n", j, i,   DMatrix[j % 3][i]);

        }

        else if (j == 0) {
          DMatrix[j % 3][i] = i;
          // printf("set %d %d %d\n", j, i,   DMatrix[j % 3][i]);

        }

        else if ((j >= 1) && (j <= n)) {


          DMatrix[j % 3][i] = MIN(DMatrix[mod((j - 1), 3)][i] + 1, MIN(DMatrix[mod((j), 3)][i - 1] + 1, DMatrix[mod((j - 1), 3)][i - 1] + (pattern[i - 1] == text[j - 1] ? 0 : 1)));
          if (i == m) {

            // printf("last line at j: %d = %d  ", j, DMatrix[j % 3][i]);
            // if (DMatrix[j % 3][i] <= k) printf("%d ", j - 1);
            // printf("\n");
          }
        }




      }
      #pragma omp barrier


    }
    #pragma omp barrier

    for ( int i = ID; i < 3; i = nthreads) {

      free(DMatrix[i]);
    }


  }


  free(DMatrix);
  t2=rdtsc();
	printf("%llu \n", t2-t1);
}




