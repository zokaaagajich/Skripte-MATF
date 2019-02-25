#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <errno.h>
#include <pthread.h>

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
    do { \
        if (!(expr)) \
            osErrorFatal(userMsg); \
    } while(0)

#define osCheckPthread(pthreadErr, userMsg)\
    do{\
    	int __pthreadErr = pthreadErr;\
    	if(__pthreadErr != 0) {\
    		errno = __pthreadErr;\
    		osAssert(false, userMsg);\
    	}\
    }while(0)


static int global_idx = -2;
static pthread_mutex_t osLock;
static pthread_cond_t osSignal;

typedef struct {
	int idx;
}data_t;

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);
void *osThreadFunction(void *arg);

int main(int argc, char **argv) {

	osAssert(2 == argc, "Invalid input!");

	int numOfThreads = atoi(argv[1]);

	pthread_t *tids = malloc(sizeof(*tids)*numOfThreads);
	osAssert(NULL != tids, "Allocating tids failed");

	data_t *threadData = malloc(sizeof(*threadData)*numOfThreads);
	osAssert(NULL != threadData, "Allocating threadData failed");

	osCheckPthread(pthread_mutex_init(&osLock, NULL), "Mutex initialization failed!");
	osCheckPthread(pthread_cond_init(&osSignal, NULL), "Cond initialization failed!");

	for(int i=0; i<numOfThreads; ++i) {
 		threadData[i].idx = i;
		osCheckPthread(pthread_create(tids+i, NULL, osThreadFunction, threadData+i), "Pthread create failed!");
	}

	int tmp_idx;

	while (true) {

		scanf("%d", &tmp_idx);
		if(-1 == tmp_idx) {
			osCheckPthread(pthread_mutex_lock(&osLock), "Mutex lock failed!");
			global_idx = tmp_idx;
			osCheckPthread(pthread_mutex_unlock(&osLock), "Mutex unlock failed!");
			osCheckPthread(pthread_cond_broadcast(&osSignal), "Cond broadcast failed!");
			break;
		}

		osCheckPthread(pthread_mutex_lock(&osLock), "Mutex lock failed!");
		global_idx = tmp_idx;
		osCheckPthread(pthread_mutex_unlock(&osLock), "Mutex unlock failed!");
		osCheckPthread(pthread_cond_broadcast(&osSignal), "Cond broadcast failed!");
	}


	for(int i=0; i<numOfThreads; i++) {
		osCheckPthread(pthread_join(tids[i], NULL), "Joining threads failed!");
	}
	
	osCheckPthread(pthread_mutex_destroy(&osLock), "Mutex destroy failed!");
	osCheckPthread(pthread_cond_destroy(&osSignal), "Cond destroy failed!");
	free(tids);
	free(threadData);

	exit(0);
}

void *osThreadFunction(void *arg) {
	
	data_t *tData = arg;

	while(true) {
		osCheckPthread(pthread_mutex_lock(&osLock), "Mutex lock failed!");
		
		while(-1 != global_idx && tData->idx != global_idx) {
			osCheckPthread(pthread_cond_wait(&osSignal, &osLock), "Wait failed!");
		}

		if(-1 == global_idx) {
			osCheckPthread(pthread_mutex_unlock(&osLock), "Mutex unlock failed!");
			break;
		}

		// THIS IS WHERE THE FUCKING MAGIC HAPPENS  
		// ja sam nit sa tim indeksom i radicu sledece interesantne stvari
		printf("%d\n", tData->idx);

		osCheckPthread(pthread_mutex_unlock(&osLock), "Mutex unlock failed!");
		global_idx = -2;
	}

	return NULL;
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
