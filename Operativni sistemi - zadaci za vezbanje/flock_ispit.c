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


void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);

int main(int argc, char **argv) {

	osAssert(4 == argc, "Invalid input: ./flock path/to/file a b");

	int fd = open(argv[1], O_RDWR);
	osAssert(-1 != fd, "Open failed!");

	struct flock lokic;
	lokic.l_type = F_WRLCK;
	lokic.l_whence = SEEK_SET;
	lokic.l_start = atoi(argv[2]);
	lokic.l_len = atoi(argv[3]) + 1;
	lokic.l_pid = 0;

	osAssert(-1 != fcntl(fd, F_GETLK, &lokic), "Fcntl failed!");

	if(lokic.l_type == F_UNLCK) {
		printf("unlocked");
	}
	else if(lokic.l_type == F_RDLCK) {
		printf("shared lock");
	}
	else if(lokic.l_type == F_WRLCK) {
		printf("exlusive lock");
	}


	close(fd);
exit(0);
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
