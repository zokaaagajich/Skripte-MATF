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
void *osCreateMemoryBlock(const char *name, unsigned size);


int main(int argc, char **argv) {

	osAssert(2 == argc, "Invalid input: ./SHM name/of/sharedM");

	unsigned memSize = sizeof(data_t);
	data_t *osData = osCreateMemoryBlock(argv[1], memSize);

	osAssert(-1 != sem_init(&(osData)->ready, PROC_SEM, 0), "Sem init failed!");
	osAssert(-1 != sem_init(&(osData)->sorted, PROC_SEM, 0), "Sem init failed!");
	
	
		srand(time(NULL));
		osData->arrayLen = rand()%10;

		for(int i=0; i<osData->arrayLen; ++i) {
			osData->array[i] = (double)rand()*10/(double)RAND_MAX;
		}

		for(int i=0; i<osData->arrayLen; ++i) {
			printf("%g ", osData->array[i]);
		}
		
		osAssert(-1 != sem_post(&(osData)->ready), "Sem post failed!");

		osAssert(-1 != sem_wait(&(osData)->sorted), "Sem wait failed!");
		
		//radi nesto
		printf("Uspesno ste sortirali!");
		//
	
		osAssert(-1 != munmap(osData, sizeof(osData)), "Munmap failed!");
		osAssert(-1 != shm_unlink(argv[1]), "Munmap failed!");

	exit(0);
}

void *osCreateMemoryBlock(const char *name, unsigned size) {

	static const mode_t defaultMode = 0644;
	int memFd = shm_open(name, O_CREAT | O_RDWR | O_EXCL, defaultMode);
	osAssert(-1 != memFd, "Shm_open failed!");

	osAssert(-1 != ftruncate(memFd, size), "Ftruncate failed!");

	void *addr;
	osAssert(MAP_FAILED !=(addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)), "Mmap failed!");
	close(memFd);	
	return addr;
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
