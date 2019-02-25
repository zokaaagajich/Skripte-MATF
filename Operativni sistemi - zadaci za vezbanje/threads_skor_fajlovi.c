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
#include <stdatomic.h>

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
    do { \
        if (!(expr)) \
            osErrorFatal(userMsg); \
    } while(0)
#define osCheckPthread(pthreadErr, userMsg)\
    do {	\
    	int __pthreadErr = pthreadErr;\
    	if(0 != __pthreadErr) { \
    		errno = __pthreadErr; \
    		osAssert(false, userMsg); \
    	}\
    } while(0)


atomic_int global_score;
void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);

void *osThreadFunction(void *arg);

int main(int argc, char **argv) {

	osAssert(argc > 1, "Invalid input: /threads path/to/fileS");

	unsigned numOfThreads = argc -1;
	pthread_t *tids = malloc(sizeof(*tids)*numOfThreads);
	osAssert(NULL != tids, "Allocating tids failed!");

	atomic_init(&global_score, 0);

	for(int i=0; i<numOfThreads; ++i) {
		osCheckPthread(pthread_create(tids+i, NULL, osThreadFunction, argv[i+1]), "Pthread creation failed!");
	}

	void *retVal;
	for(int i=0; i<numOfThreads; ++i){
		osCheckPthread(pthread_join(tids[i], &retVal), "Joining threads failed!");
		printf("%d thread: %d\n", i, *(int *)retVal);
		free(retVal);
	}

	printf("Global: %d\n", atomic_load(&global_score));
	free(tids);	

exit(0);
}

void *osThreadFunction(void *arg) {

	char *fpath = arg;

	FILE *file = fopen(fpath, "r");
	osAssert(NULL!=file, "Opening file failed!");

	int numOfPlus = 0, numOfMinus = 0;
	char c;

	while(fscanf(file, "%c", &c) != EOF) {
		if(c == '+') 
			numOfPlus++;
		else if(c == '-')
			numOfMinus++; 
	}
	
	fclose(file);
	
	int *retVal = malloc(sizeof(int));
	osAssert(NULL != retVal, "Allocating return value failed!");
	*retVal = numOfPlus - numOfMinus;

	atomic_fetch_add(&global_score, *retVal);
	return retVal;
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
