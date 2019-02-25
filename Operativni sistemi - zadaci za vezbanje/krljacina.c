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
#include <errno.h>
#include <string.h>

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

#define MAX_LEN 128

static int global_number = -2;
static char global_buffer_1;
static char global_buffer_2;

pthread_mutex_t osLock;
pthread_cond_t osSignal;

typedef struct {
	int idx;
	int start_idx;
	int end_idx;
	FILE *file;
} data_t;

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);
void *osThreadFunction(void *arg);

// putanja do fajla i broj threadova, svaki thread dobija k-tinu
// u besk petlji se vrtim i scanujem broj i ako je -1 break u suprotnom cita se dva slova, iste duzine, 
// ako nisu ne prekidati program samo ispis
// ako jesu iste saljem tredu koji ima indeks tog broja mora da u svom delu fajla zameni rec drugom 
// nacin na koji ih menja: zakljuca rec sa flock, zameni je, otkljuca i ide dalje
// imam malu pomoc: pwrite
// svaki tred ima povratnu vrednost i vracamo koliko je puta zamenio neko govno

int main(int argc, char **argv) {

	osAssert(3 == argc, "Invalid input: ./threads numOfThreads path/to/file");

	unsigned numOfThreads = atoi(argv[1]);
	pthread_t *tids = malloc(sizeof(*tids)*numOfThreads);
	osAssert(NULL != tids, "Allocation of tids failed!");

	data_t *threadData = malloc(sizeof(*threadData)*numOfThreads);
	osAssert(NULL != threadData, "Allocation of data thread failed!");

	FILE *file = fopen(argv[2], "r+");
	osAssert(NULL != file, "Fopen failed!");

	osCheckPthread(pthread_mutex_init(&osLock, NULL), "Mutex initialization failed!");
	osCheckPthread(pthread_cond_init(&osSignal, NULL), "Cond initialization failed!");

	struct stat fInfo;
	osAssert(-1 != stat(argv[2], &fInfo), "Stat failed!");

	int step = fInfo.st_size/numOfThreads;
	int start = 0;
	int remain = fInfo.st_size % numOfThreads;

	for(int i=0; i<numOfThreads; ++i) {
		
		threadData[i].start_idx = start; 
		start+=step;
		threadData[i].end_idx = start;
		
		threadData[i].idx = i;
		
		if(i == numOfThreads - 1) 
			threadData[i].end_idx += remain;

		threadData[i].file = file;
		osCheckPthread(pthread_create(tids+i, NULL, osThreadFunction, threadData+i), "Thread creation failed!");
	}


	int local_number;
	char buffer_1;
	char buffer_2;

	while(true) {

		scanf("%d", &local_number);
		getchar();

		if(-1 == local_number) {
			osCheckPthread(pthread_mutex_lock(&osLock), "Mutex lock failed!");
				global_number = local_number;
			osCheckPthread(pthread_mutex_unlock(&osLock), "Mutex unlock failed!");
			osCheckPthread(pthread_cond_broadcast(&osSignal), "Cond broadcast failed!");
			break;
		}

		while(true){
			scanf("%c %c", &buffer_1, &buffer_2);
			getchar();

			if(buffer_1 == buffer_2) {
				printf("Buffers must be different!\n");
			} 
			else {
				break;
			}
		}

		osCheckPthread(pthread_mutex_lock(&osLock), "Mutex lock failed!");
			global_number = local_number;
			global_buffer_1 = buffer_1;
			global_buffer_2 = buffer_2;
		osCheckPthread(pthread_mutex_unlock(&osLock), "Mutex unlock failed!");
		osCheckPthread(pthread_cond_broadcast(&osSignal), "Cond broadcast failed!");
	}


	void *retVal;
	for(int i=0; i<numOfThreads; ++i) {
		osCheckPthread(pthread_join(tids[i], &retVal), "Joining threads failed!");
		printf("%d.thread: %d\n", i, *(int *)retVal);
		free(retVal);
	}

	free(tids);
	free(threadData);
	osCheckPthread(pthread_mutex_destroy(&osLock), "Mutex destroy failed!");
	osCheckPthread(pthread_cond_destroy(&osSignal), "Cond destroy failed!");
	
	exit(0);
}

void *osThreadFunction(void *arg) {

	data_t *pThreadData = arg;
	int local_brojac = 0;

	while(true) {
		
		osCheckPthread(pthread_mutex_lock(&osLock), "Mutex lock failed!");

		while(-1!=global_number && global_number != pThreadData->idx) {
			osCheckPthread(pthread_cond_wait(&osSignal, &osLock), "Cond wait failed!");
		}

		if(-1 == global_number) {
			osCheckPthread(pthread_mutex_unlock(&osLock), "Mutex unlock failed!");
			break;
		}

		//na mene je red da radim
		osAssert(-1 != fseek(pThreadData->file, pThreadData->start_idx, SEEK_SET),"Fseek failed!");
		int n = pThreadData->end_idx - pThreadData->start_idx;
		char c;

		for(int i=0; i<n; ++i) {
			fscanf(pThreadData->file, "%c", &c);
			if(c == global_buffer_1) {
				osAssert(-1 != fseek(pThreadData->file, -1, SEEK_CUR),"Fseek failed!");
				fprintf(pThreadData->file, "%c", global_buffer_2);
				local_brojac++;
			}
		}

		osCheckPthread(pthread_mutex_unlock(&osLock), "Mutex unlock failed!");
		global_number = -2;
	}


	int *retVal = malloc(sizeof(int));
	osAssert(NULL != retVal, "Allocation of return value failed!");
	*retVal = local_brojac;

	return retVal;
}
void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
