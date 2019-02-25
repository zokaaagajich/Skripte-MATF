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
    do {\
    	int __pthreadErr = pthreadErr;\
    	if(0 != __pthreadErr) {\
    		errno = __pthreadErr;\
    		osAssert(false, userMsg);\
    	}\
    }while(false)

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
} color_pixel_t;

typedef struct {
	int idx;
}data_t;

static color_pixel_t **global_matrix;
static int global_m;

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);

color_pixel_t **allocateMatrix(color_pixel_t **A, int n, int m);
void deallocateMatrix(color_pixel_t **A, int n);
void loadMatrix(color_pixel_t **A, int n, int m, FILE *file);
void writeMatrix(color_pixel_t **A, int n, int m);

void *osThreadFunction(void *arg);

int main(int argc, char **argv) {
	
	osAssert(2 == argc, "Invalid input: ./a.out path/to/file ");

	FILE *file = fopen(argv[1], "r");
	osAssert(NULL != file, "Opening file failed!");

	int n, m;
	fscanf(file, "%d %d", &m, &n);	
	
	global_matrix = allocateMatrix(global_matrix, n, m);
	loadMatrix(global_matrix, n, m, file);
	//writeMatrix(global_matrix, n, m);

	unsigned numOfThreads = n;
	pthread_t *tids = malloc(sizeof(*tids)*numOfThreads);
	osAssert(NULL != tids, "Allocating threads failed!");

	data_t *threadData = malloc(sizeof(*threadData)*numOfThreads);
	osAssert(NULL != tids, "Allocating thread data failed!");

	global_m = m;

	for(int i=0; i<numOfThreads; ++i) {
		threadData[i].idx = i;
		osCheckPthread(pthread_create(tids+i, NULL, osThreadFunction, threadData+i), "Thread creation failed!");
	}	

	void *retVal;
	for(int i=0; i<numOfThreads; ++i) {
		osCheckPthread(pthread_join(tids[i], &retVal), "Joining threads failed!");
			unsigned char *ret = retVal;
			for(int j=0; j<m; ++j) {
				printf("%d ", ret[j]);
			}
		printf("\n");
	}

	deallocateMatrix(global_matrix, n);
	free(tids);
	free(threadData);
	fclose(file);

	exit(0);
}

void *osThreadFunction(void *arg) {
	
	data_t *threadData = arg;

	unsigned char *x = malloc(sizeof(unsigned char)*global_m);
	osAssert(NULL != x, "Allocation of retVal failed!");

	for(int i=0; i<global_m; ++i) {
		x[i] = global_matrix[threadData->idx][i].r * 0.3 + global_matrix[threadData->idx][i].g *0.59 + global_matrix[threadData->idx][i].b * 0.11;
	}

	return x;
}

color_pixel_t **allocateMatrix(color_pixel_t **A, int n, int m) {
	A = malloc(sizeof(color_pixel_t *)*n);
	osAssert(NULL != A, "Allocation of matrix failed!");
	for(int i=0; i<n; ++i) {
		A[i] = malloc(sizeof(color_pixel_t)*m);
		osAssert(NULL != A[i], "Allocation of matrix failed!");	
	}
	return A;
}

void deallocateMatrix(color_pixel_t **A, int n) {
	for(int i=0; i<n; ++i) {
		free(A[i]);
	}
	free(A);
	A = NULL;
}

void loadMatrix(color_pixel_t **A, int n, int m, FILE *file) {
	for(int i=0; i<n; ++i) {
		for(int j=0; j<m; ++j) {
			fscanf(file, "%hhu %hhu %hhu", &(A[i][j]).r, &(A[i][j]).g, &(A[i][j]).b);
		}
	}
}

void writeMatrix(color_pixel_t **A, int n, int m) {
	for(int i=0; i<n; ++i) {
		for(int j=0; j<m; ++j) {
			printf("%hhu %hhu %hhu ",A[i][j].r, A[i][j].g, A[i][j].b);
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
