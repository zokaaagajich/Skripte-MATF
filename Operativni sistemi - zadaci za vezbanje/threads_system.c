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
#include <string.h>

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
    do { \
        if (!(expr)) \
            osErrorFatal(userMsg); \
    } while(0)
#define osCheckPthread(pthreadErr, userMsg)\
    do {\
    	int __pthreadErr = pthreadErr;\
    	if(0!=__pthreadErr) {\
    		errno = __pthreadErr;\
    		osAssert(false, userMsg);\
    	}\
    }while(false)

#define MAX 1024

pthread_mutex_t osLock;
pthread_cond_t osSignal;
static char global_command[MAX];
static int global_idx = -2;

typedef struct {
	int idx;
}data_t;

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);
void *osThreadFunction(void *arg);

int main(int argc, char **argv) {
	
	osAssert(2 == argc, "Invalid input: ./threads numOfThreads");

	unsigned numOfThreads = atoi(argv[1]);
	osAssert(numOfThreads > 0 && numOfThreads < 40, "Number of threads must be positive and less then 40!");

	pthread_t *tids = malloc(sizeof(*tids)*numOfThreads);
	osAssert(NULL != tids, "Allocation of tids failed!");

	data_t *threadData = malloc(sizeof(*threadData)*numOfThreads);
	osAssert(NULL != threadData, "Allocation of thread data failed!");

	osCheckPthread(pthread_mutex_init(&osLock, NULL), "Mutex initialization failed!");
	osCheckPthread(pthread_cond_init(&osSignal, NULL), "Cond initialization failed!");

	for(int i = 0; i<numOfThreads; ++i) {
		threadData[i].idx = i;
		osCheckPthread(pthread_create(tids+i, NULL, osThreadFunction, threadData+i), "Pthread creation failed!");
	}


	int local_idx;

	while(true) {

		scanf("%d", &local_idx);
		if(-1 == local_idx) {
			osCheckPthread(pthread_mutex_lock(&osLock), "Mutex locking failed!");
				global_idx = local_idx;
			osCheckPthread(pthread_mutex_unlock(&osLock), "Mutex unlocking failed!");
			osCheckPthread(pthread_cond_broadcast(&osSignal), "Cond broadcast failed!");
			break;
		}
		
		size_t len = 0;
		char *buf = NULL;
		osAssert(-1!=getline(&buf, &len, stdin), "Getline failed!");
		
		
		osCheckPthread(pthread_mutex_lock(&osLock), "Mutex locking failed!");
				global_idx = local_idx;
				strcpy(global_command, buf); 
		osCheckPthread(pthread_mutex_unlock(&osLock), "Mutex unlocking failed!");
		osCheckPthread(pthread_cond_broadcast(&osSignal), "Cond broadcast failed!");
	
	}

	void *retVal;
	for(int i=0; i<numOfThreads; ++i) {
		osCheckPthread(pthread_join(tids[i],&retVal), "Joining threads failed!");
		printf("%d.thread: %s\n", i, (char *)retVal);
		free(retVal);
	}

	osCheckPthread(pthread_mutex_destroy(&osLock), "Mutex destroy failed!");
	osCheckPthread(pthread_cond_destroy(&osSignal), "Cond destroy failed!");
	free(tids);
	free(threadData);

	exit(0);
}

void *osThreadFunction(void *arg) {

	data_t *threadData = arg;

	while(true) {

		osCheckPthread(pthread_mutex_lock(&osLock), "Mutex locking failed!");
		while(-1!=global_idx && global_idx != threadData->idx) {
			osCheckPthread(pthread_cond_wait(&osSignal, &osLock), "Cond wait failed!");
		}

		if(-1 == global_idx) {
			osCheckPthread(pthread_mutex_unlock(&osLock), "Mutex unlocking failed!");
			break;
		}

		// na mene je red
		char *retVal = malloc(sizeof(char)*MAX);
		osAssert(NULL != retVal, "Allocating retVal failed!");
	
		int status;
		osAssert(-1 != (status = system(global_command)), "system failed!");

		if(0 != status) {
			strcpy(retVal, "Neuspesno!\0");
		}
		else {
			strcpy(retVal, "Uspesno!\0");
		}
		
		global_idx = -2;		
		osCheckPthread(pthread_mutex_unlock(&osLock), "Mutex unlocking failed!");
		return retVal;
	}
}


void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
