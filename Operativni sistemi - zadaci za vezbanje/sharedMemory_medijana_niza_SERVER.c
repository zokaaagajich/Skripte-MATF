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
#include <time.h>

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
void *osCreateMemoryBlock(const char *name, unsigned size);

int main(int argc, char **argv) {

	osAssert(2 == argc, "Invalid input: ./shared name/of/shared/block");


	unsigned mem_size = (sizeof(osInputData));
	osInputData *inputData = osCreateMemoryBlock(argv[1], mem_size);

	osAssert(-1!=sem_init(&(inputData)->inDataReady, PROC_SEM, 0), "Sem init failed!");


	inputData->arrayLen = rand() % 10;
	srand(time(NULL));
	for(int i=0; i<(inputData->arrayLen); ++i) {
		inputData->array[i] = (float)rand()/(float)RAND_MAX;
		printf("%g ", inputData->array[i]);
	}

	osAssert(-1!=sem_post(&(inputData)->inDataReady), "Sem post failed!");

	osAssert(-1!=munmap(inputData, sizeof(inputData)), "Munmap failed!");
	exit(0);
}

void *osCreateMemoryBlock(const char *name, unsigned size) {

	static const mode_t defaultMode = 0644;
	int memFd = shm_open(name, O_CREAT | O_RDWR, defaultMode);
	osAssert(-1!=memFd, "Shm_open failed!");

	osAssert(-1!=ftruncate(memFd, size), "Ftruncate failed!");

	void *addr;
	osAssert(MAP_FAILED != (addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)),"Mmap failed");
	close(memFd);
	return addr;
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
