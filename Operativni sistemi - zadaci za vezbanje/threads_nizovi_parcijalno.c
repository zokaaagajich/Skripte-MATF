#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
    do { \
        if (!(expr)) \
            osErrorFatal(userMsg); \
    } while(0)

#define osCheckPthread(pthreadErr, userMsg);\
    do { \
    	int __pthreadErr = (pthreadErr);\
    		if(__pthreadErr!=0) {\
    			errno = __pthreadErr;\
    			osAssert(false, userMsg);\
    		}\
    } while(0)

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);
void *osThreadFunction(void *arg);

pthread_mutex_t osLock; 
int osMinimum;
int *array = NULL;

typedef struct {
	int start_idx;
	int end_idx;
} data_t;

int main(int argc, char **argv) {
	/**
	 *  Generisemo niz, i delimo threadove da rade sa podeljenim nizom, ako imamo niz 
	 *  1 2 3 4 5 6 7, prvi thread radi sa 1 i 2, drugi sa 3 i 4 i trei sa 5, 6, 7
	 *  Trazimo minimum niza i mutexom ga uptadujemo
	 */

	osAssert(3 == argc, "Invalid input: ./threads numOfThreads dimension/of/array");

	unsigned numOfThreads = atoi(argv[1]);
	pthread_t *tids = malloc(sizeof(*tids)*numOfThreads);
	osAssert(NULL != tids, "Allocation of tids failed");

	int n = atoi(argv[2]);
	array = malloc(sizeof(int)*n);
	osAssert(NULL!=array, "Allocation of array failed!");

	srand(time(NULL));
	for(int i=0; i<n; i++) {
		array[i] = rand() % 1000;
	}

	osMinimum = array[0];

	data_t *pthreadData = malloc(sizeof(*pthreadData) * numOfThreads);
	osAssert(NULL != pthreadData, "Allocation of pthread data failed!"); 

	osCheckPthread(pthread_mutex_init(&osLock, NULL), "Pthread mutex init failed!");

	int step = n/numOfThreads;
	int setter = 0;
	int ostatak = n%numOfThreads;

	for(int i=0; i<numOfThreads; ++i) {

		pthreadData[i].start_idx = setter;
		setter += step;
		pthreadData[i].end_idx = setter;

		if(i == numOfThreads - 1)
			pthreadData[i].end_idx += ostatak;
		
		osCheckPthread(pthread_create(tids+i, NULL, osThreadFunction, pthreadData+i), "Creation threads failed!");
	}

	for(int i=0; i<numOfThreads; ++i) {
		osCheckPthread(pthread_join(tids[i], NULL), "Joining pthreads failed!");
	}

	for(int i=0; i<n; ++i)
		printf("%d ", array[i]);
	printf("\nMINIMUM: %d\n", osMinimum);

	osCheckPthread(pthread_mutex_destroy(&osLock), "Pthread mutex init failed!");
	free(array);
	free(tids);
	free(pthreadData);

exit(0);
}

void *osThreadFunction(void *arg) {

	data_t *osData = arg;
	int min_local = array[osData->start_idx];

	for (int i=osData->start_idx + 1; i < osData->end_idx; ++i)
		if(array[i] < min_local) 
			min_local = array[i];
	
	osCheckPthread(pthread_mutex_lock(&osLock), "Mutex lock failed");
		if(min_local < osMinimum) 
			osMinimum = min_local;
	osCheckPthread(pthread_mutex_unlock(&osLock), "Mutex unlock failed!");
	
	return NULL;
}	

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
