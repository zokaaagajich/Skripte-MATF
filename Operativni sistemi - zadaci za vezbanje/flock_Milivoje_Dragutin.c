/* SVAKU REC MILIVOJE IZ FAJLA MENJA NA DRAGUTIN. sta je olaksano, pa olakano nam jer su nam reci iste duzine
	pa mozemo lepo to da odradimo.
*/
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

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
    do { \
        if (!(expr)) \
            osErrorFatal(userMsg); \
    } while(0)


void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);

int main(int argc, char **argv) {

	osAssert(2 == argc, "Invalid input: ./flock path/to/file");

	FILE *file = fopen(argv[1], "r+");
	osAssert(NULL != file, "Fopen failed!");

	int fd = fileno(file);
	osAssert(-1 != fd, "Fileno failed!");

	char buf[126];

	struct flock lokic;
	lokic.l_type = F_WRLCK;
	lokic.l_whence = SEEK_SET;
	lokic.l_len = 8; //duzina stringa MILIVOJE

	while(1 == fscanf(file, "%s", buf)) {
		if(strcmp(buf, "Milivoje") == 0) {
			osAssert(-1 != fseek(file, -8, SEEK_CUR), "Fseek failed!");
			
			lokic.l_start = ftell(file);
			lokic.l_pid = 0;

			osAssert(-1!=fcntl(fd, F_GETLK, &lokic), "Fcntl getlck failed!");
				
				if(lokic.l_type == F_WRLCK) {
					printf("w %jd\n", (intmax_t)lokic.l_pid);
				}
			
				else if(locik.l_type == F_RDLCK) {
					printf("r %jd\n", (intmax_t)lokic.l_pid);
				}

				else {
					osAssert(-1 != fcntl(fd, F_SETLK, &lokic), "Fcntl failed!");
		
					fprintf(file, "%s", "Dragutin");

					lokic.l_type = F_UNLCK;
					osAssert(-1 != fcntl(fd, F_SETLK, &lokic), "Fcntl unlock failed!");
				}
		}
	}

	fclose(file);
	exit(0);
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
