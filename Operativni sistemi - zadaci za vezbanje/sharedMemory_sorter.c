#define _XOPEN_SOURCE 700

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <time.h>
#include <semaphore.h>
#include <sys/mman.h>

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
    do { \
        if (!(expr)) \
            osErrorFatal(userMsg); \
    } while(0)

#define MAX_LEN 1024
#define LOCAL_SEM (0)
#define PROC_SEM (!LOCAL_SEM)

typedef struct {
  sem_t ready;
  sem_t sorted;
  double array[MAX_LEN];
  size_t arrayLen;
}data_t;

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);
void *osGetMemoryBlock(const char *name, int *blockSize);
int osUporedi (const void *a, const void *b);

int main(int argc, char **argv) {

  osAssert(2 == argc, "Invalid input: ./SHM name/of/sharedM");

  int blockSize;
  data_t *osData = osGetMemoryBlock(argv[1], &blockSize);

  osAssert(-1 != sem_wait(&(osData)->ready), "Sem wait failed!");

  qsort(osData->array, osData->arrayLen, sizeof(double), osUporedi);

    for(int i=0; i<osData->arrayLen; ++i) {
      printf("%g ", osData->array[i]);
    }
    
  osAssert(-1 != sem_post(&(osData)->sorted), "Sem post failed!");
  osAssert(-1 != munmap(osData, sizeof(osData)), "Munmap failed!");

  exit(0);
}

void *osGetMemoryBlock(const char *name, int *blockSize) {

  static const mode_t defaultMode = 0644;
  int memFd = shm_open(name, O_RDWR, defaultMode);
  osAssert(-1 != memFd, "Shm_open failed!");

  struct stat fInfo;
  osAssert(-1 != fstat(memFd, &fInfo), "Stat failed!");
  *blockSize = fInfo.st_size;

  void *addr;
  osAssert(MAP_FAILED != (addr = mmap(NULL, *blockSize, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)), "Mmap failed!");
  close(memFd); 
  return addr;
}

int osUporedi (const void *a, const void *b) {
  double A = *(double *)a;
  double B = *(double *)b;

  return (A<B) - (A>B);
}
void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
