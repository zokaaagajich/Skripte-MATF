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
    do { \
    	int __pthreadErr = pthreadErr;\
    	if(0 != __pthreadErr) {\
    		errno = __pthreadErr;\
    		osAssert(false, userMsg);\
    	} \
    }while(false)

static double **global_matrix_A = NULL;
static double **global_matrix_B = NULL;
static double global_sum;
pthread_mutex_t osLock;

typedef struct {
	int n;
	int idx_row;
} data_t;

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);

void *osThreadFunction(void *arg);
double **allocateMatrix(double **A, int n);
void loadMatrix(double **A, int n, FILE *stream);
void dealocateMatrix(double **A, int n);
void writeMatrix(double **A, int n);

int main(int argc, char **argv) {

	osAssert(2 == argc, "Invalid input: ./matrix path/to/file");

	FILE *file = fopen(argv[1], "r");
	osAssert(NULL != file, "Opening file failed!");

	int n;
	fscanf(file, "%d", &n);
	osAssert(n > 0, "Dimension of matrix is supposed to be positive number!");

	global_matrix_A = allocateMatrix(global_matrix_A, n);
	global_matrix_B = allocateMatrix(global_matrix_B, n);

	loadMatrix(global_matrix_A, n, file);
	loadMatrix(global_matrix_B, n, file);

	fclose(file);

	osCheckPthread(pthread_mutex_init(&osLock, NULL), "Mutex initialization failed!");

	unsigned numOfThreads = n;
	pthread_t *tids = malloc(sizeof(*tids)*numOfThreads);
	osAssert(NULL != tids, "Allocating threads failed!");

	data_t *pthreadData = malloc(sizeof(*pthreadData)*numOfThreads);
	osAssert(NULL != pthreadData, "Allocating threads data failed!");

	global_sum = 0;

	for(int i=0; i<numOfThreads; ++i) {
		pthreadData[i].n = n;
		pthreadData[i].idx_row = i;
		osCheckPthread(pthread_create(tids+i, NULL, osThreadFunction, pthreadData + i), "Pthread creation failed!");
	}


	for(int i=0; i<numOfThreads; ++i) {
		osCheckPthread(pthread_join(tids[i], NULL), "Joining threads failed!");
	}

	printf("Global: %g\n", global_sum);
	osCheckPthread(pthread_mutex_destroy(&osLock), "Mutex destroy failed!");
	
	dealocateMatrix(global_matrix_A,n);
	dealocateMatrix(global_matrix_B, n);
	free(tids);
	free(pthreadData);

	exit(0);
}

void *osThreadFunction(void *arg) { 

	data_t *threadData = arg;
	double local_sum = 0;

	for(int i=0; i<threadData->n; ++i) {
		local_sum += global_matrix_A[threadData->idx_row][i] * global_matrix_B[threadData->idx_row][i];
	}

	printf("L: %g\n", local_sum);

	osCheckPthread(pthread_mutex_lock(&osLock), "Locking mutex failed!");
		global_sum += local_sum;
	osCheckPthread(pthread_mutex_unlock(&osLock), "Unlocking mutex failed!");

return NULL;
}

void writeMatrix(double **A, int n) {
	for(int i=0; i<n; ++i) {
		for(int j=0; j<n; ++j) {
			printf("%g ", A[i][j]);
		}
	printf("\n");
	}
}
void dealocateMatrix(double **A, int n) {
	for(int i=0; i<n; ++i) {
		free(A[i]);
	}
	free(A);
	A = NULL;
}

void loadMatrix(double **A, int n, FILE *file) {

	for(int i=0; i<n; ++i) {
		for(int j=0; j<n; ++j) {
			fscanf(file, "%lf", &A[i][j]);
		}
	}
}

double **allocateMatrix(double **A, int n) {

	A = malloc(sizeof(double *)*n);
	osAssert(NULL != A, "Allocating matrix failed!");

	for(int i=0; i<n; ++i) {
		A[i] = malloc(sizeof(double)*n);
		osAssert(NULL != A[i], "Allocating matrix failed!");
	}

	return A;
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
