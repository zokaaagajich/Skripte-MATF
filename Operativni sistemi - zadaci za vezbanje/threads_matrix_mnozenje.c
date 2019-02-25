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
    	int __pthreadErr = (pthreadErr);\
    	if(0 != __pthreadErr) {\
    		errno = __pthreadErr;\
    		osAssert(false, userMsg);\
    	}\
    } while(false)

static double  **global_matrix_A = NULL;
static double  **global_matrix_B = NULL;
static double  **global_matrix_C = NULL;

typedef struct {
	int idx_row;
	int idx_col;
	int n;
} data_t;

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);

double **osAllocateMatrix(double **A, int n);
void osLoadMatrix (double **A, int n, FILE *stream);
void osDealocateMatrix (double **A, int n);
void osWriteMatrix(double **A, int n);
void *osThreadFunction(void *arg);

int main(int argc, char **argv) {

	osAssert(2 == argc, "Invalid input!");

	FILE *file = fopen(argv[1], "r");
	osAssert(NULL!=file, "Fopen failed!");

	int n;
	fscanf(file, "%d", &n);
	printf("%d\n", n);

	osAssert(n > 0, "Invalid dimension of matrix!");

	global_matrix_A = osAllocateMatrix(global_matrix_A,n);
	global_matrix_B = osAllocateMatrix(global_matrix_B,n);
	global_matrix_C = osAllocateMatrix(global_matrix_C,n);

	osLoadMatrix(global_matrix_A, n, file);
	osLoadMatrix(global_matrix_B, n, file);

	fclose(file);

	int numOfThreads = n;
	pthread_t **tids = malloc(sizeof(pthread_t *)*numOfThreads);
	osAssert(NULL!=tids, "Malloc with tids failed!");

	for (int i=0; i<numOfThreads; ++i) {
		tids[i] = malloc(sizeof(pthread_t)*numOfThreads);
		osAssert(NULL != tids[i], "Malloc with tids failed!");
	}


	data_t **osThreadData = malloc(sizeof(data_t *)*numOfThreads); // ne zaboravi *pointer i da pomnozis sa n
	osAssert(NULL!=osThreadData, "Malloc with data_t failed!");
	
	for(int i=0; i<n; ++i) {
		osThreadData[i] = malloc(sizeof(data_t)*numOfThreads);
		osAssert(NULL != osThreadData[i], "Malloca with data_t failed");
	}


		for(int i = 0; i < n; ++i) {
			for(int j = 0;j < n; ++j) {
				osThreadData[i][j].idx_row = i;
				osThreadData[i][j].idx_col = j;
				osThreadData[i][j].n = n;
				osCheckPthread(pthread_create(&tids[i][j], NULL, osThreadFunction, &osThreadData[i][j]), "pthread_create failed");
			}
		}


	for(int i=0; i<numOfThreads; ++i) {
		for(int j=0; j<numOfThreads; ++j) {
			osCheckPthread(pthread_join(tids[i][j], NULL), "Joining failed!");
		}
	}


	osWriteMatrix(global_matrix_C, n);

	for(int i=0; i<numOfThreads; i++){
		free(tids[i]);
		free(osThreadData[i]);
	}

	free(tids);
	free(osThreadData);
	osDealocateMatrix(global_matrix_A, n);
	osDealocateMatrix(global_matrix_B, n);
	osDealocateMatrix(global_matrix_C, n);

exit(0);
}

void *osThreadFunction(void *arg) {

	data_t *osData = arg;

	double local = 0;

	for(int i=0; i < (osData->n); ++i) {
		local += global_matrix_A[osData->idx_row][i] * global_matrix_B[i][osData->idx_col];
	}

	global_matrix_C[osData->idx_row][osData->idx_col] = local; 
	return NULL;
}

void osWriteMatrix(double **A, int n) {
	for(int i = 0; i<n; ++i) {
		for(int j=0; j<n; ++j) 
			printf("%g ", A[i][j]);
	printf("\n");
	}
}
void osDealocateMatrix (double **A, int n) {
	for(int i=0; i<n; ++i)
		free(A[i]);
	
	free(A);
	A = NULL;
}

void osLoadMatrix (double **A, int n, FILE *stream) {
	for(int i=0; i<n; ++i){
		for(int j=0; j<n; ++j){ 
			fscanf(stream, "%lf", &A[i][j]);
		}
	}
}

double **osAllocateMatrix(double **A, int n) {
	A = malloc(sizeof(double *)*n);
	osAssert(NULL!=A, "Malloc failed!");

	for(int i=0; i<n; ++i) {
		A[i] = malloc(sizeof(double)*n);
		osAssert(NULL!=A[i], "malloc failed!");
	}

	return A;
}
void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
