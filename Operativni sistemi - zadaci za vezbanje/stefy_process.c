#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <pthread.h>
#include <stdatomic.h>
#include <sys/mman.h>
#include <errno.h>
#include <semaphore.h>

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
    do { \
        if (!(expr)) \
            osErrorFatal(userMsg); \
    } while(false)
#define osCheckPthread(pthreadErr, userMsg)\
    do {\
      int __pthreadErr = pthreadErr;\
      if(0 != __pthreadErr) {\
        errno = __pthreadErr;\
        osAssert(false, userMsg);\
      }\
    } while(false)


#define LOCAL_SEM (0)
#define PROC_SEM (!LOCAL_SEM)

#define _MAX 100

typedef struct {
  sem_t ready;
  sem_t processed;
  double matrix[_MAX][_MAX];
  size_t rows;
  size_t cols;
  atomic_int sum;
} data_t;

typedef struct {
  int idx;
  data_t *shared;
} tdata_t;

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);

void *osGetMemBlock(const char *name, int *blockSize);
void writeMatrix(double A[_MAX][_MAX], int n, int m);
void *osThreadFunction(void *arg);

int main(int argc, char **argv) {
  
    osAssert(2 == argc, "Invalid input: ./stefy sharedName ... -lrt -lpthread");

    int blockSize;
    data_t *pData = osGetMemBlock(argv[1], &blockSize);
    osAssert(-1 != sem_wait(&(pData)->ready), "Semwait failed!");

    // writeMatrix(pData->matrix, pData->rows, pData->cols);

    int numOfThreads = pData->rows;
    pthread_t *tids = malloc(sizeof(*tids)*numOfThreads);
    osAssert(NULL != tids, "Allocation of threads failed!");

    tdata_t *tData = malloc(sizeof(*tData)*numOfThreads);
    osAssert(NULL != tData, "Allocation of tData failed");

    for(int i=0; i<numOfThreads; ++i) {
      tData[i].idx = i;
      tData[i].shared = pData;
      osCheckPthread(pthread_create(tids + i, NULL, osThreadFunction, tData + i), "Creating threads failed!");
    }

    for(int i=0; i<numOfThreads; ++i) {
      osCheckPthread(pthread_join(tids[i], NULL), "Joining threads failed!");
    }

    free(tids);
    free(tData);
    osAssert(-1 != sem_post(&(pData)->processed), "Semwait failed!"); 

exit(0);
}

void *osThreadFunction(void *arg) {
  
  tdata_t *tData = arg;
  int local_sum = 0;

  for(size_t i=0; i < tData->shared->cols; ++i) {
    local_sum += tData->shared->matrix[tData->idx][i];
  }

  atomic_fetch_add(&(tData)->shared->sum, local_sum);
  return NULL;
}

void *osGetMemBlock(const char *name, int *blockSize) {

    static const mode_t defaultMode = 0644;
    int memFd = shm_open(name, O_RDWR, defaultMode);
    osAssert(-1 != memFd, "Shm_open failed to work!");

    struct stat fInfo;
    osAssert(-1 != fstat(memFd, &fInfo), "Fstat failed!");
    *blockSize = fInfo.st_size; 

    void *addr;
    osAssert(MAP_FAILED != (addr = mmap(NULL, *blockSize, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)), "Mmap failed to work!");

    close(memFd);
    return addr;
}


void writeMatrix(double A[_MAX][_MAX], int n, int m) {
   for(int i=0; i<n; ++i) {
    for(int j=0; j<m; ++j) {
      printf("%g ", A[i][j]);
    }
    printf("\n");
  }
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
