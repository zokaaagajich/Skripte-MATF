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
	 
   osAssert(3 == argc, "Invalid input: ./flock path/to/file a/0");

   FILE *file = fopen(argv[1], "r+");
   osAssert(NULL != file, "Opening file failed!");

   int fd = open(argv[1], O_RDWR);
   osAssert(-1 != fd, "Fdopen failed!");

  struct flock lokic;
   lokic.l_type = F_WRLCK;
   lokic.l_whence = SEEK_SET;
   lokic.l_len = 1;

  char character;
    if(argv[2][0] == 'a') {  //All characters in file are changed to the next
                             //by ASCII, numbers and special characters do not change!
          while(1 == fscanf(file, "%c", &character)) {
            if((character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z' )) {

                   if(character == 'z' || character == 'Z') {
                        character = character - 25;
                   }
                   else {
                        character = character + 1;
                   }

              osAssert(-1 != fseek(file, -1, SEEK_CUR), "Fseek failed!");
              lokic.l_start = ftell(file);

              lokic.l_type = F_WRLCK;
              osAssert(-1 != fcntl(fd, F_SETLK, &lokic), "Fcntl failed to lock!");
            
              fprintf(file, "%c", character);
              
              lokic.l_type = F_UNLCK;
              osAssert(-1 != fcntl(fd, F_SETLK, &lokic), "Fcntl failed to unlock!");
            }
          }
    }

    else if(argv[2][0] == '0') {
          while(1 == fscanf(file, "%c", &character)) {
            if((character >= '0') && (character <= '9')) {

                   if(character == '9') {
                        character = character - 9;
                   }
                   else {
                        character = character + 1;
                   }

              osAssert(-1 != fseek(file, -1, SEEK_CUR), "Fseek failed!");
              lokic.l_start = ftell(file);

              lokic.l_type = F_WRLCK;
              osAssert(-1 != fcntl(fd, F_SETLK, &lokic), "Fcntl failed to lock!");
            
              fprintf(file, "%c", character);
              
              lokic.l_type = F_UNLCK;
              osAssert(-1 != fcntl(fd, F_SETLK, &lokic), "Fcntl failed to unlock!");
            }
          }
    }


    fclose(file);
    close(fd);
exit(0);
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
