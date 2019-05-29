#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define BILLION 1000000000L

int main(int argc, char *argv[])
{
	double elapsedTime;
	struct timespec start, end;

	// START CLOCK
	clock_gettime(CLOCK_MONOTONIC, &start);

    // 1000 OPERATIONS
	int i = 0;
	for (i ; i < 1000 ; i++)
	{
		i = getpid();
	}

	// END CLOCK
	clock_gettime(CLOCK_MONOTONIC, &end);

	// AVERAGE ELAPSED TIME OP
	double secStart = (double) start.tv_sec;
	double nanosecStart = (double) start.tv_nsec;
	double secEnd = (double) end.tv_sec;
	double nanosecEnd = (double) end.tv_nsec;
	
	elapsedTime = ((BILLION * (secEnd - secStart)) + (nanosecEnd - nanosecStart));
	elapsedTime /= 10000;

	printf("\nAvg. time for getpid() system call is %lf nanoseconds.\n", elapsedTime);

	return 0;

}
