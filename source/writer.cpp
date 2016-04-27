#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <signal.h>
#include <semaphore.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <math.h>
#include <time.h>
#include <string.h>
#include "os3.h"
using namespace std;

char memname[10];

void signal_handler(int signum)
{
	// Shared segment unlink: Remove a named posix shared segment from the system.
        if ( shm_unlink(memname) < 0 )
        {
         perror("shm_unlink");
        }

	// Closing all named semaphores
        if ( sem_close(access_sem) < 0 )
        {
         perror("sem_close");
        }

        if ( sem_close(readers_sem) < 0 )
        {
         perror("sem_close");
        }

        if ( sem_close(order_sem) < 0 )
        {
         perror("sem_close");
        }

        if ( sem_close(readerscount_sem) < 0 )
        {
         perror("sem_close");
        }

	// Semaphore unlink: Remove all named semaphores from the system.
        if ( sem_unlink("access") < 0 )
        {
         perror("sem_unlink");
        }

        if ( sem_unlink("readers") < 0 )
        {
         perror("sem_unlink");
        }

        if ( sem_unlink("order") < 0 )
        {
         perror("sem_unlink");
        }

        if ( sem_unlink("readers_counter") < 0 )
        {
         perror("sem_unlink");
        }

   	// Terminate program with the signal number as output
   	exit(signum);
}

int main(int argc, char *argv[]) {


	int shmfd;
	int shared_seg_size = (50 * sizeof(struct shared_data)); /* want shared segment capable of storing 1 message */
	struct shared_data* shared_msg; 
	timespec ts;
	struct stat sb;

	int lb, ub, period, count;

	if (argc < 7)
	{
		cout << "Call the writer as: ./writer -r lb ub -d period -s shmid\n";
		exit(-1);
	}


	for (int i = 0; i < argc; i++)
	{
		if (strcmp (argv[i],"-r") == 0) 
		{
			lb = atoi(argv[i+1]);
			ub = atoi(argv[i+2]);	
		}
		else if (strcmp (argv[i],"-d") == 0)
		{
			period = atoi(argv[i+1]);
		}
		else if (strcmp (argv[i],"-s") == 0)
		{
			strcpy(memname,argv[i+1]);
		}
	} 	


	// open the shared memory object with shm_open() - read, write, execute/search by owner and group 
	shmfd = shm_open(memname, O_RDWR, S_IRWXU | S_IRWXG);
	if (shmfd < 0)
	{
		perror("In shm_open()");
		exit(1);
	}


	// Open semaphores
	access_sem=sem_open("access", S_IRUSR | S_IWUSR);
	order_sem=sem_open("order",  S_IRUSR | S_IWUSR);

	sem_wait(order_sem);
	sem_wait(access_sem);
	sem_post(order_sem);
	//write

	// mapping the shared memory in the virtual address space of the process
	shared_msg = (struct shared_data*)mmap(NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
	if (shared_msg == NULL)
	{
		perror("In mmap()");
		if ( shm_unlink(memname) < 0 )
		{
		 perror("shm_unlink");
		}
		exit(1);
	}
	printf( "Shared memory segment allocated correctly (%d bytes).\n", shared_seg_size);

	cout << "\n Before Writing: Current contens in shared memory: \n";
	for(int i = lb; i < ub; i++)
	{
		cout<<"PID: "<<shared_msg[i].var1<<"\n";
		cout<<shared_msg[i].var2<<"\n";
		printf("Position modified at %lld.%.9ld seconds\n", (long long)shared_msg[i].var3.tv_sec, shared_msg[i].var3.tv_nsec);
	}

	for (int i = lb; i < ub; i++)
	{
		shared_msg[i].var1 = getpid();
		if ( i % 2 == 0)
		{
			sprintf(shared_msg[i].var2,"Hello Dear Reader! My message is: trifillara!");
		}
		else
		{
			sprintf(shared_msg[i].var2,"Hello Dear Reader! My message is: omadara!");
		}	
		clock_gettime(CLOCK_REALTIME, &ts);
		shared_msg[i].var3 = ts;
	}

	cout << "\n After Writing: Current contens in shared memory: \n";
	for(int i = lb; i < ub; i++)
	{
		cout<<"PID: "<<shared_msg[i].var1<<"\n";
		cout<<shared_msg[i].var2<<"\n";
		printf("Position modified at %lld.%.9ld seconds\n", (long long)shared_msg[i].var3.tv_sec, shared_msg[i].var3.tv_nsec);
	}

	if (fstat(shmfd, &sb) == -1) 
	{
		perror("stat");
		exit(EXIT_FAILURE);
	}

	printf("\nShared memory size:	%lld bytes\n",(long long) sb.st_size);
        printf("Blocks allocated:	%lld\n",(long long) sb.st_blocks);

	cout << "Ok now i am going to take a nap\n";
	sleep(period);
	sem_post(access_sem);

	cout << "Closing all the semaphores i have opened\n";
        if ( sem_close(access_sem) < 0 )
        {
         perror("sem_close");
        }

        if ( sem_close(order_sem) < 0 )
        {
         perror("sem_close");
        }


}
