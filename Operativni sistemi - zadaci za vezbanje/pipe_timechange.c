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
#include <utime.h>

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
    do { \
        if (!(expr)) \
            osErrorFatal(userMsg); \
    } while(0)


void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);
#define RD_END (0)
#define WR_END (1)
#define MAX (100)

int main(int argc, char **argv) {

  osAssert(1 == argc, "Invalid input!");

  int pipeFds[2];
  osAssert(-1 != pipe(pipeFds), "Pipe failed!");

  pid_t childPid = fork();
  osAssert(-1!=childPid, "Fork failed!");

     if(childPid > 0) { //parent
        close(pipeFds[RD_END]);
      
          char file[MAX];
          char naredba[MAX];
          int broj_necega;
          
          scanf("%s %s %d", file, naredba, &broj_necega);
        
          FILE *stream = fdopen(pipeFds[WR_END],"w");
          osAssert(NULL != stream, "Fdopen failed!");

          fprintf(stream, "%s %s %d", file, naredba, broj_necega);

          fclose(stream);
     }
     else { //child
          close(pipeFds[WR_END]);

          FILE *stream = fdopen(pipeFds[RD_END],"r");
          osAssert(NULL != stream, "Fdopen failed!");

          char file[MAX];
          char naredba[MAX];
          int broj_necega;

          fscanf(stream, "%s %s %d", file, naredba, &broj_necega);
          int fd = open(file, O_CREAT | O_EXCL | O_RDWR);
          osAssert(-1 != fd, "Creating file failed!"); 

          if(strcmp(naredba, "velicina")==0) {
            osAssert(-1 != ftruncate(fd, broj_necega), "Ftruncate failed!");
            printf("Kreirao se fajls sa imenom: %s i postavio se na velicinu %d bajtova!\n", file, broj_necega);
          }
          
          else if(strcmp(naredba, "vreme")==0) {

          	struct stat fInfo;
          	osAssert(-1 != stat(file, &fInfo), "stat failed!");

            struct utimbuf times;
            times.actime = fInfo.st_size; //TODO
            times.modtime = broj_necega;
            
            osAssert(-1 != utime(file, &times), "Utime failed!");
            printf("Kreirao se fajls sa imenom: %s i poslednje vreme mod na %d sekundi!\n", file, broj_necega);

          }

          fclose(stream);
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
