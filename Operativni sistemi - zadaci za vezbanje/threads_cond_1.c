#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <pthread.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
    do { \
        if (!(expr)) \
            osErrorFatal(userMsg); \
    } while(false)

#define osCheckPthread(pthreadErr, userMsg)\
    do{\
    	int __pthreadErr = pthreadErr;\
    	if(0 != __pthreadErr) {\
    		errno = __pthreadErr;\
    		osAssert(false, userMsg);\
    	}\
    } while(false)

#define MAX_LEN 127

pthread_mutex_t osLock;
pthread_cond_t osSignal;

static char globalWord[MAX_LEN];
static int globalNumber = -2; 

typedef struct {
	int idx;
} data_t;

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);
void *osThreadFunction (void *arg);
int countLower(char *a);

int main(int argc, char **argv) {

	osAssert(argc == 2, "Invalid input: ./threads numOfThreads");

	unsigned numOfThreads = atoi(argv[1]);
	pthread_t *tids = malloc(sizeof(*tids)*numOfThreads);
	osAssert(NULL != tids, "Allocation of threads failed!");

	data_t *pthreadData = malloc(sizeof(*pthreadData)*numOfThreads);
	osAssert(NULL != pthreadData, "Allocation of thread data failed!");

	osCheckPthread(pthread_mutex_init(&osLock, NULL), "Mutex initialization failed!");
	osCheckPthread(pthread_cond_init(&osSignal, NULL), "Cond initialization failed!");

	for(int i = 0; i<numOfThreads; ++i) {
		pthreadData[i].idx = i;
		osCheckPthread(pthread_create(tids+i, NULL, osThreadFunction, pthreadData+i), "Thread creation failed!");
	}


	int local_num;
	char local_word[MAX_LEN];

	while(true) {	

		scanf("%s %d", local_word, &local_num);
	
		if(local_num == -1) {
			osCheckPthread(pthread_mutex_lock(&osLock), "Mutex lock failed!");
			globalNumber = local_num;
			osCheckPthread(pthread_mutex_unlock(&osLock), "Mutex unlock failed!");
			osCheckPthread(pthread_cond_broadcast(&osSignal), "Cond broadcast failed!");
			break;
		}

		osCheckPthread(pthread_mutex_lock(&osLock), "Mutex lock failed!");
		
		globalNumber = local_num;
		strcpy(globalWord, local_word);

		osCheckPthread(pthread_mutex_unlock(&osLock), "Mutex unlock failed!");
		osCheckPthread(pthread_cond_broadcast(&osSignal), "Cond broadcast failed!");
	}	


	for(int i=0; i<numOfThreads; ++i) {
		osCheckPthread(pthread_join(tids[i], NULL), "Joining threads failed!");
	}

	osCheckPthread(pthread_mutex_destroy(&osLock), "Mutex destroy failed!");
	osCheckPthread(pthread_cond_destroy(&osSignal), "Cond destroy failed!");

	free(tids);
	free(pthreadData);

exit(0);
}

void *osThreadFunction (void *arg) {
	
	data_t *pthreadData = arg;
	
	while(true) {

		osCheckPthread(pthread_mutex_lock(&osLock), "Mutex lock failed!");

		while(globalNumber != -1 && globalNumber != pthreadData->idx) {
			osCheckPthread(pthread_cond_wait(&osSignal, &osLock), "Waiting on cond wait failed!");
		}

		if(-1 == globalNumber) {
			osCheckPthread(pthread_mutex_unlock(&osLock), "Mutex unlock failed!");
			break;
		}	

	// ako je na njega red onda zelimo da izracunamo broj malih slova
		
		int local_brojac = countLower(globalWord);
		printf("%d.pthread: %d\n", pthreadData->idx, local_brojac);

		osCheckPthread(pthread_mutex_unlock(&osLock), "Mutex unlock failed!");
		globalNumber = -2;
	}


return NULL;
}

int countLower(char a[MAX_LEN]) {
	int local_br = 0;

	for(int i = 0; i<strlen(a); ++i) {
		if(islower(a[i])) {
			local_br++;
		}
	}

	return local_br;
}
void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
