#define _XOPEN_SOURCE 700
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
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
int palindrom(char s[MAX_LEN]);

int main(int argc, char **argv) {
	
  osAssert(2 == argc, "Invalid input: ./ numOfFIFOs");
  
  int numOfFIFOs = atoi(argv[1]);
  struct pollfd *fileMonitors = malloc(sizeof(*fileMonitors)*numOfFIFOs);
  osAssert(NULL != fileMonitors, "Allocation failed!");

  char tmp[8] = "/tmp/_\0";

  for(int i=0; i<numOfFIFOs; ++i) {
    sprintf(tmp+5, "%d%c", i, '\0');
    fileMonitors[i].fd = open(tmp, O_RDONLY | O_NONBLOCK);
    osAssert(-1 != fileMonitors[i].fd, "Opening failed!");
    fileMonitors[i].events |= POLLIN; 
  }

  int numOfFIFOs_left = numOfFIFOs;

  while(numOfFIFOs_left != 0) {
    int event = poll(fileMonitors, numOfFIFOs, -1);
    osAssert(-1 != event, "Poll failed!");

    for(int i=0; i<numOfFIFOs; ++i) {

      if(fileMonitors[i].revents & POLLIN) {
      
        char buf[MAX_LEN];
       
        FILE *stream = fdopen(fileMonitors[i].fd, "r");
        osAssert(NULL != stream, "Fdopen failed!");

        fscanf(stream, "%s", buf);
          printf("%s\n", buf);
        
        
        if(palindrom(buf)) 
          printf("/tmp/%d: rec: %s. Jeste palindrom!\n", i,buf);
        else  
          printf("/tmp/%d: rec: %s. Nije palindrom!\n", i,buf);

        fclose(stream);
        fileMonitors[i].revents = 0;
        numOfFIFOs_left--; //mozda ne treba

      }

      else if((fileMonitors[i].revents & POLLHUP) || (fileMonitors[i].revents & POLLERR)) {
        numOfFIFOs_left--;
        close(fileMonitors[i].fd);
      }

    }
  }
 
exit(0);
}

int palindrom(char s[MAX_LEN]) {
  int i, j;
  int n = strlen(s)-1;

  for(i=0, j=n; i<n/2; i++, j--)
    if(s[i]!=s[j])
      return 0;
  return 1;
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
