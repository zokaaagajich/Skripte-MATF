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

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
    do { \
        if (!(expr)) \
            osErrorFatal(userMsg); \
    } while(0)

#define osCheckPthread(pthreadErr, userMsg)\
    do { \
    	int __pthreadErr = pthreadErr;\
    	if(0 != __pthreadErr) {\
    		errno = __pthreadErr;\
    		osAssert(false, userMsg);\
    	}\
    } while(0)

static pthread_mutex_t osLock;
static int global_min;
static int **global_matrix = NULL;

typedef struct {
	int m;
	int idx_row;
}data_t;


void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);
int **allocateMatrix(int **A, int n, int m);
void loadMatrix(int **A, int n, int m, FILE *stream);
void deallocateMatrix(int **A, int n, int m);
void writeMatrix(int **A, int n, int m);
void *osThreadFunction(void *arg);

int main(int argc, char **argv) {

	osAssert(2 == argc, "Invalid input: ./threads file/to/path/with/matrix");

	FILE *file = fopen(argv[1], "r");
	osAssert(NULL != file, "Opening file failed!");

	int n, m;
	fscanf(file, "%d %d", &n, &m);
	osAssert(n>0 && m>0, "Incorrect dimension of matrix!");

	global_matrix = allocateMatrix(global_matrix, n, m);
	loadMatrix(global_matrix, n, m, file);
	fclose(file);
	writeMatrix(global_matrix, n, m);

	unsigned numOfThreads = n;
	pthread_t *tids = malloc(sizeof(*tids)*numOfThreads);
	osAssert(NULL != tids, "Allocating threads failed!");

	data_t *threadData = malloc(sizeof(*threadData)*numOfThreads);
	osAssert(NULL != threadData, "Allocating thread data failed!");

	osCheckPthread(pthread_mutex_init(&osLock, NULL), "Mutex init failed!");
	global_min = global_matrix[0][0];

	for(int i=0; i<numOfThreads; ++i) {
		threadData[i].idx_row = i;
		threadData[i].m = m;
		osCheckPthread(pthread_create(tids+i, NULL, osThreadFunction, threadData+i), "Thread creation failed!");
	}

	for(int i=0; i<numOfThreads; ++i){
		osCheckPthread(pthread_join(tids[i], NULL), "Joining threads failed!");
	}

	printf("MINIMUM: %d\n", global_min);

	free(tids);
	free(threadData);
	deallocateMatrix(global_matrix,n,m);
	osCheckPthread(pthread_mutex_destroy(&osLock), "Mutex destroy failed!");

exit(0);
}

void *osThreadFunction(void *arg) {
	data_t *threadData = arg;

	int local_min = global_matrix[threadData->idx_row][0];

	for(int i=0; i<threadData->m; ++i) {
		if(global_matrix[threadData->idx_row][i] < local_min) {
			local_min = global_matrix[threadData->idx_row][i];
		}
	}

	osCheckPthread(pthread_mutex_lock(&osLock), "Locking mutex failed!");
		if(local_min < global_min)
			global_min = local_min;
	osCheckPthread(pthread_mutex_unlock(&osLock), "Unlocking mutex failed!");
	return NULL;
}

int **allocateMatrix(int **A, int n, int m){
	A = malloc(sizeof(int *)*n);
	osAssert(NULL != A, "Allocating matrix failed!");

	for(int i=0; i<m; ++i){
		A[i] = malloc(sizeof(int)*m);
		osAssert(NULL != A[i], "Allocating matrix failed!");
	}

	return A;
}

void loadMatrix(int **A, int n, int m, FILE *stream) {

	for(int i=0; i<n; ++i) {
		for(int j=0; j<m; ++j) {
			fscanf(stream, "%d", &A[i][j]);
		}
	}
}

void deallocateMatrix(int **A, int n, int m) {

	for(int i=0; i<n; ++i) {
		free(A[i]);
	}
	free(A);
	A = NULL;
}

void writeMatrix(int **A, int n, int m) {
	for(int i=0; i<n; ++i) {
		for(int j=0; j<m; ++j) {
			printf("%d ", A[i][j]);
		}
	printf("\n");
	}
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
