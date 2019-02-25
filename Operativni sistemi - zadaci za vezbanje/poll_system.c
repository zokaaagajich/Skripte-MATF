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

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
    do { \
        if (!(expr)) \
            osErrorFatal(userMsg); \
    } while(0)

#define MAX_LEN 127
void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);

int main(int argc, char **argv) {
	
	osAssert(2 == argc, "Invalid input: ./ num of FIFOS");
	//fifos look like /tmp/0 ... 

	int numOfFIFOs = atoi(argv[1]);
	osAssert(numOfFIFOs > 0, "Number of FIFOS must be positive!");

	struct pollfd *fileMonitors = malloc(sizeof(*fileMonitors)*numOfFIFOs);
	osAssert(NULL != fileMonitors, "Allocation of fileMonitors failed!");

	char tmp[8] = "/tmp/_\0";
	for(int i=0; i<numOfFIFOs; ++i) {
		sprintf(tmp + 5, "%d%c", i,'\0');
		fileMonitors[i].fd = open(tmp, O_RDONLY | O_NONBLOCK);
		osAssert(-1 != fileMonitors[i].fd, "Opening failed!");
		fileMonitors[i].events |= POLLIN;
	}

	
	unsigned numOfFIFOs_left = numOfFIFOs;
	while(numOfFIFOs_left != 0) {
		int event = poll(fileMonitors, numOfFIFOs, -1);
		osAssert(-1 != event, "Poll failed!");

		for(int i=0; i<numOfFIFOs; ++i) {
			if (fileMonitors[i].revents & POLLIN) {

				char *command = NULL;
				size_t readBytes = 0;

				FILE *stream = fdopen(fileMonitors[i].fd, "r");
				osAssert(NULL != stream, "Fdopen failed!");

				osAssert(-1 != getline(&command, &readBytes, stream), "Read failed!");
				int status;
				
				osAssert(-1 != (status = system(command)), "System failed!");
				if(0 != status) {
					printf("Neuspesno!\n");
				}	
				else {
					printf("Uspesno!");
				}
				
				fileMonitors[i].revents = 0;
				//numOfFIFOs_left--; 
				fclose(stream);
			}
			else if((POLLHUP | POLLERR) & fileMonitors[i].revents) {
					numOfFIFOs_left--;
					close(fileMonitors[i].fd);
			}
		}
	}

	

exit(0);
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
