#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include <signal.h>

#define osErrorFatal(userMsg) osErrorFatalImpl((userMsg), __FILE__, __func__, __LINE__)
#define osAssert(expr, userMsg) \
    do { \
        if (!(expr)) \
            osErrorFatal(userMsg); \
    } while(0)

static unsigned count_USR1 = 0;
static unsigned count_USR2 = 0;

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);
void signalHandler(int signal);

int main(int argc, char **argv) {

	osAssert(1 == argc, "Invalid input!");

	printf("%jd\n", (intmax_t)getpid());

	osAssert(SIG_ERR != signal(SIGUSR1, signalHandler), "Handle signal USR1 failed!");
	osAssert(SIG_ERR != signal(SIGUSR2, signalHandler), "Handle signal USR2 failed!");
	osAssert(SIG_ERR != signal(SIGTERM, signalHandler), "Handle signal TERM failed!");

	while(true) {
		osAssert(SIG_ERR != signal(SIGUSR1, signalHandler), "Handle signal USR1 failed!");
		osAssert(SIG_ERR != signal(SIGUSR2, signalHandler), "Handle signal USR2 failed!");
		osAssert(SIG_ERR != signal(SIGTERM, signalHandler), "Handle signal TERM failed!");
	}

exit(0);
}

void signalHandler(int signal) {

	if(signal == SIGUSR1) {
		count_USR1++;
	}

	else if(signal == SIGUSR2) {
		count_USR2++;
	}

	else if(signal == SIGTERM) {
		printf("%u %u\n", count_USR1, count_USR2);
		exit(0);
	}
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
