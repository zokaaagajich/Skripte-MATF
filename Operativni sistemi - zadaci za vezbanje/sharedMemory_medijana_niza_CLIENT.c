#define _XOPEN_SOURCE 700


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <semaphore.h>
#include <sys/mman.h>

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
    do { \
        if (!(expr)) \
            osErrorFatal(userMsg); \
    } while(0)

#define ARRAY_MAX 1024
#define LOCAL_SEM (0)
#define PROC_SEM (!LOCAL_SEM)

typedef struct {
	sem_t inDataReady;
	float array[ARRAY_MAX];
	unsigned arrayLen;
} osInputData;

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);
void *osGetMemoryBlock(const char *name, int *blockSize);
int osUporedi(const void *a, const void *b);

int main(int argc, char **argv) {

	osAssert(2 == argc, "Invalid input: ./shared name/of/shared/block");

	int blockSize;	 
	osInputData *pInputData = osGetMemoryBlock(argv[1], &blockSize);
	osAssert(-1!=sem_wait(&(pInputData)->inDataReady), "Sem wait failed!");

	qsort(pInputData->array, pInputData->arrayLen, sizeof(float), osUporedi);
		
	printf("%g\n", pInputData->array[(pInputData->arrayLen)/2]);

	osAssert(-1!=munmap(pInputData, sizeof(pInputData)), "Munmap failed!");
	
	exit(0);
}

void *osGetMemoryBlock(const char *name, int *blockSize) {

	int memFd = shm_open(name, O_RDWR, 0644);
	osAssert(-1 != memFd, "Shm_open failed!");

	struct stat fInfo;
	osAssert(-1 != fstat(memFd, &fInfo), "Fstat failed!");
	*blockSize = fInfo.st_size;

	void *addr;
	osAssert(MAP_FAILED != (addr = mmap(NULL, *blockSize, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)),"Mmap failed");
	close(memFd);
	return addr;
}

int osUporedi(const void *a, const void *b) {
	float A = *(float *)a;
	float B = *(float *)b;

	return (A < B)-(A > B);
}
void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
