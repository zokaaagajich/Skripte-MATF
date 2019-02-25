#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
    do { \
        if (!(expr)) \
            osErrorFatal(userMsg); \
    } while(0)

#define RD_END (0)
#define WR_END (1)

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);

int main(int argc, char **argv) {

	osAssert(1 == argc, "Invalid input!");

	int pipeFds[2];
	osAssert(-1!=pipe(pipeFds), "Pipe failed!");

	pid_t childPid = fork();
	osAssert(-1 != childPid, "Fork failed!");


	if(childPid > 0) { //parent
		close(pipeFds[WR_END]);

		FILE *stream = fdopen(pipeFds[RD_END],"r");
		osAssert(NULL != stream, "Fdopen failed!");

		char *buf = NULL;
		size_t len = 0;

		osAssert(-1!=getline(&buf,&len,stream), "Getline failed!");

		int size, number; 
		while(fscanf(stream, "%s %d %s %s %d %s %d %s %s", buf, &number, buf, buf, &size, buf, &number, buf, buf)!=EOF) {
			printf("%d\n", size);
		}

		fclose(stream); //closes fd
	}
	else { //child
		close(pipeFds[RD_END]);

		osAssert(-1!=dup2(pipeFds[WR_END], STDOUT_FILENO), "Redirecting STDOUT failed");
		osAssert(-1!=execlp("ls", "ls", "-l", NULL), "Exec failed!");
		
		exit(0);
	}	

exit(0);
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
