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

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);

void *osCreateMemBlock(const char *name, unsigned size);
void LoadMatrix (double A[_MAX][_MAX], int n, int m, FILE *file);
void writeMatrix(double A[_MAX][_MAX], int n, int m);

int main(int argc, char **argv) {
	
    osAssert(3 == argc, "Invalid input: ./stefy sharedName path/to/file/withMatrix .. -lrt -lpthread");

    data_t *pData = osCreateMemBlock(argv[1], sizeof(data_t));

    osAssert(-1 != sem_init(&(pData)->ready, PROC_SEM, 0), "First semaphore failed to initialize!");
    osAssert(-1 != sem_init(&(pData)->processed, PROC_SEM, 0), "Second semaphore failed to initialize!");
    atomic_init(&(pData)->sum, 0);

    FILE *file = fopen(argv[2], "r");
    osAssert(NULL != file, "Opening file failed!");

    fscanf(file, "%lu%lu", &(pData)->rows, &(pData)->cols);    
    LoadMatrix(pData->matrix, pData->rows, pData->cols, file);
    //writeMatrix(pData->matrix, pData->rows, pData->cols);
    fclose(file);

    osAssert(-1 != sem_post(&(pData)->ready), "Sempost failed!");
    osAssert(-1 != sem_wait(&(pData)->processed), "Semwait failed!");

    printf("ATOMIC: %d\n", atomic_load(&(pData)->sum));

    osAssert(-1 != munmap(pData, sizeof(pData)), "Munmap failed!");
    osAssert(-1 != shm_unlink(argv[1]), "Shm_unlink failed!");
    
exit(0);
}

void *osCreateMemBlock(const char *name, unsigned size) {

    static const mode_t defaultMode = 0644;
    int memFd = shm_open(name, O_CREAT | O_EXCL | O_RDWR, defaultMode);
    osAssert(-1 != memFd, "Shm_open failed to work!");

    osAssert(-1 != ftruncate(memFd, size), "Ftruncate failed to work his magic!");

    void *addr;
    osAssert(MAP_FAILED != (addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)), "Mmap failed to work!");

    close(memFd);
    return addr;
}

void LoadMatrix (double A[_MAX][_MAX], int n, int m, FILE *file) {
  for(int i=0; i<n; ++i) {
    for(int j=0; j<m; ++j) {
      fscanf(file, "%lf", &A[i][j]);
    }
  }
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
