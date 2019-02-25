#define _XOPEN_SOURCE 700

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <poll.h>
#include <string.h>

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
    do { \
        if (!(expr)) \
            osErrorFatal(userMsg); \
    } while(0)

#define MAX_LEN 256
static int global_size = -1;
static int global_file_idx;

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);

int main(int argc, char **argv) {

	osAssert(2 == argc, "Invalid input: ./poll numOfFifos fifo1.. fifoN");

	unsigned numOfFIFOs = atoi(argv[1]);
	struct pollfd *fileMonitors = malloc(sizeof(*fileMonitors)*numOfFIFOs);
	osAssert(NULL != fileMonitors, "Allocating fileMonitors failed!");

	char tmp[8] = "/tmp/_\0";

	for(int i=0; i<numOfFIFOs; ++i) {
		sprintf(tmp + 5, "%d%c", i, '\0');
		fileMonitors[i].fd = open(tmp, O_RDONLY | O_NONBLOCK);
		osAssert(-1 != fileMonitors[i].fd, "Opening fifo failed!");
		fileMonitors[i].events |= POLLIN; 
	}

	unsigned fifoLeft = numOfFIFOs;

	while(fifoLeft != 0) {
		int event = poll(fileMonitors, numOfFIFOs, -1);
		osAssert(-1 != event, "Poll failed!");

		for(int i=0; i<numOfFIFOs; ++i) {
			if(fileMonitors[i].revents & POLLIN) {

				char *buf = NULL;
				size_t len = 0;
				int readBytes = 0;

				FILE *stream = fdopen(fileMonitors[i].fd, "r");
				osAssert(NULL != stream, "Fdopen failed!");

				osAssert(-1 != (readBytes = getline(&buf, &len, stream)), "Memory allocation failed");
				
				if(readBytes > global_size) {
					global_size = readBytes;
					global_file_idx = i;
				}

				fileMonitors[i].revents = 0;
			} 

			else if ((POLLERR | POLLHUP) & fileMonitors[i].revents) {
				fifoLeft--;
				close(fileMonitors[i].fd);
			}
		}
	}

	printf("/tmp/%d %d\n",global_file_idx, global_size);

	free(fileMonitors);

exit(0);
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
