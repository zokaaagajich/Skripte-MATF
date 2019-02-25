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

static int global_number;
void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum);
void signalerHandler(int signal);

int main(int argc, char **argv) {

	osAssert(1 == argc, "Invalid input!");

	printf("pid %jd\n", (intmax_t)getpid());

	osAssert(SIG_ERR != signal(SIGUSR1, signalerHandler), "Failed to handle signal USR1!");
	osAssert(SIG_ERR != signal(SIGUSR2, signalerHandler), "Failed to handle signal USR2!");
	osAssert(SIG_ERR != signal(SIGINT, signalerHandler), "Failed to handle signal INT!");
	osAssert(SIG_ERR != signal(SIGQUIT, signalerHandler), "Failed to handle signal QUIT!");

	while(1) {
		scanf("%d", &global_number);
		
		osAssert(SIG_ERR != signal(SIGUSR1, signalerHandler), "Failed to handle signal USR1!");
		osAssert(SIG_ERR != signal(SIGUSR2, signalerHandler), "Failed to handle signal USR2!");
		osAssert(SIG_ERR != signal(SIGINT, signalerHandler), "Failed to handle signal INT!");
		osAssert(SIG_ERR != signal(SIGQUIT, signalerHandler), "Failed to handle signal QUIT!");
	}

exit(0);
}


void signalerHandler(int signal) {
	
	if(signal == SIGUSR1) {
		printf("%d\n", global_number*global_number);
	}

	else if(signal == SIGUSR2) {
		printf("%d\n", global_number*global_number*global_number);
	}
	
	else if(signal == SIGINT) {
		int cifra, r_broj = 0;

		while (global_number != 0) {
			cifra=global_number%10;
			r_broj = r_broj*10 + cifra;
			global_number/=10;
		}
		
		printf("%d\n", r_broj);
	}

	else if(signal == SIGQUIT) {
		exit(0);
	}
}

void osErrorFatalImpl(const char *userMsg, const char *fileName, 
                      const char *functionName, const int lineNum) {
    perror(userMsg);
    fprintf(stderr, "File: '%s'\nFunction: '%s'\nLine: '%d'\n", fileName, functionName, lineNum);
    exit(EXIT_FAILURE);
}
