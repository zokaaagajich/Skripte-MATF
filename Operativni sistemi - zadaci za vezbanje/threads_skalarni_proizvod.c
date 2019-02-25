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
    } while(false)
#define osCheckPthread(pthreadErr, userMsg)\
    do {\
    	int __pthreadErr = pthreadErr;\
    	if(0 != __pthreadErr) {\
    		errno = __pthreadErr;\
    		osAssert(false, userMsg);\
    	}\
    }while(false)

pthread_mutex_t osLock;
static int global_sk;
static int *vector_1 = NULL;
static int *vector_2 = NULL;

typedef struct {
	int idx;
}data_t;

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);
int *allocateArray(int *A, int n);
void loadString(int *A, int n, FILE *file);
void writeArray(int *A, int n);
void *osThreadFunction(void *arg);

int main(int argc, char **argv) {
	osAssert(2 == argc, "Invalid input: ./thread path/to/file");

	FILE *file = fopen(argv[1], "r");
	osAssert(NULL != file, "Opening file failed!");

	unsigned numOfThreads;
	fscanf(file, "%d", &numOfThreads);
	osAssert((numOfThreads > 0) && (numOfThreads < 100), "Invalid input of vector dimnesion!");

	vector_1 = allocateArray(vector_1, numOfThreads);
	vector_2 = allocateArray(vector_2, numOfThreads);	
	loadString(vector_1, numOfThreads, file);
	loadString(vector_2, numOfThreads, file);

	writeArray(vector_1, numOfThreads);
	writeArray(vector_2, numOfThreads);
	
	osCheckPthread(pthread_mutex_init(&osLock, NULL), "Mutex init failed!");

	pthread_t *tids = malloc(sizeof(*tids)*numOfThreads);
	osAssert(NULL != tids, "Allocating threads failed!");
	data_t *threadData = malloc(sizeof(*threadData)*numOfThreads);
	osAssert(NULL != threadData, "Allocating thread data failed!");

	global_sk = 0;

	for(int i=0; i<numOfThreads; ++i) {
		threadData[i].idx = i;
		osCheckPthread(pthread_create(tids+i, NULL, osThreadFunction, threadData +i), "Creating threads failed!");
	}

	void *retVal;
	for(int i=0; i<numOfThreads; ++i) {
		osCheckPthread(pthread_join(tids[i], &retVal), "Joining threads failed!");
		printf("%d.thread: %d\n", i, *(int *)retVal);
		free(retVal);
	}

	printf("Global: %d\n", global_sk);

	free(vector_1);
	free(vector_2);
	free(tids);
	free(threadData);
	osCheckPthread(pthread_mutex_destroy(&osLock), "Mutex destroy failed!");

exit(0);
}

void *osThreadFunction(void *arg) {

	data_t *threadData = arg;
	int local_multiply = vector_1[threadData->idx]*vector_2[threadData->idx]; 

	osCheckPthread(pthread_mutex_lock(&osLock), "Mutex locking failed!");
		global_sk += local_multiply;
	osCheckPthread(pthread_mutex_unlock(&osLock), "Mutex unlocking failed!");

	int *retVal = malloc(sizeof(int));
	osAssert(NULL != retVal, "Allocating return value failed!");
	*retVal = local_multiply;
	return retVal;
}

void writeArray(int *A, int n) {
	for(int i=0; i<n; ++i) {
		printf("%d ", A[i]);
	}
	printf("\n");
}
int *allocateArray(int *A, int n) {
	A = malloc(sizeof(int)*n);
	osAssert(NULL != A, "Allocating array failed!");
	return A;
}

void loadString(int *A, int n, FILE *file) {
	for(int i=0; i<n; ++i) {
		fscanf(file, "%d", &A[i]);
	}
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
