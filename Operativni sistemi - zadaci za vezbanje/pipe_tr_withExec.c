#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/wait.h>

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

	osAssert(argc == 2, "Invalid input: ./pipe path/to/file");

	int pipe_PC[2];
	int pipe_CP[2];

	osAssert(-1!=pipe(pipe_PC), "Creating pipe P-C failed!");
	osAssert(-1!=pipe(pipe_CP), "Creating pipe C-P failed!");

	pid_t childPid = fork();
	osAssert(-1!=childPid, "Fork failed!");

	if(childPid > 0) {  //parent
	close(pipe_PC[RD_END]);
	close(pipe_CP[WR_END]);

	struct stat fInfo;
	osAssert(-1!=stat(argv[1], &fInfo), "Stat failed!");

	size_t size = fInfo.st_size + 1;
	char *buf = malloc(size);
	osAssert(NULL != buf, "Allocating buffer failed!");

	int fd = open(argv[1], O_RDONLY);
	osAssert(-1!=fd, "Opening fd failed!");

	int readBytes = 0;	
	osAssert(-1 != (readBytes = read(fd, buf, size)), "Reading from file failed!");
	osAssert(-1 != write(pipe_PC[WR_END], buf, readBytes), "Writing to child failed!");
	close(pipe_PC[WR_END]);

	osAssert(-1 != (readBytes = read(pipe_CP[RD_END], buf, size)), "Reading from pipe CP failed!");
	printf("%s\n", buf);

	int status = 0;
	osAssert(-1!=wait(&status), "Waiting on child failed!");
	if(WEXITSTATUS(status)==EXIT_FAILURE) {
		printf("Neuspeh!");
	}

	free(buf);
	close(pipe_CP[RD_END]);
	}

	else { //child
	close(pipe_PC[WR_END]);
	close(pipe_CP[RD_END]);

	osAssert(-1!=dup2(pipe_PC[RD_END], STDIN_FILENO), "Redirecting STDIN failed!");
	osAssert(-1!=dup2(pipe_CP[WR_END], STDOUT_FILENO), "Redirecting STDOUT failed!");

	osAssert(-1!=execlp("tr", "tr", "a", "A", NULL), "Exec failed!");
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
