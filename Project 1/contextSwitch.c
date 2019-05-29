#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <pthread.h>



#include <sched.h>
#include <sys/types.h>

#define BILLION 1000000000L

int main (int argc, char *argv[])
{
	double elapsedTime;
	struct timespec start, end;

	// THREAD AFFINITY : ONE PROCESSOR
	cpu_set_t my_cpu_set;
	CPU_ZERO(&my_cpu_set);
	CPU_SET(0, &my_cpu_set);

	sched_setaffinity(0, sizeof(cpu_set_t),&my_cpu_set);
	
	int pipe1[2];
	int pipe2[2];
	int byteL;
	char connectionString[] = ".\n";
	pid_t child_processID;
	char buffer[5];
	int i, j;	

	pipe(pipe1);
	pipe(pipe2);
	if ((child_processID = fork()) == -1)
	{
		perror("fork");
		exit(1);
	}

	// START CLOCK
	clock_gettime(CLOCK_MONOTONIC, &start);

	
	if (child_processID == 0)
	{
		close(pipe1[0]);
		close(pipe2[1]);
		
		write(pipe1[1], connectionString, (strlen(connectionString)+1));

		for (i = 0; i < 1000; i++)
		{
			read(pipe2[0], buffer, sizeof(buffer));
			write(pipe1[1], buffer, sizeof(buffer));
		}

		exit(0);
	}

	else
	{
		close(pipe1[1]);
		close(pipe2[0]);
		
		for (j = 0; j < 1000; j++)
		{
			read(pipe1[0], buffer, sizeof(buffer));
			write(pipe2[1], buffer, sizeof(buffer));
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
	
		//Printing time elapsed for context switch
		printf("\nAvg. time taken for context switch is %lf nanoseconds.\n", elapsedTime);
	}

return 0;
}
