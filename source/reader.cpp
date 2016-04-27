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
#include <string.h>
#include <math.h>
#include <time.h>
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
	int vol, cur;
	int shared_seg_size = (50 * sizeof(struct shared_data)); 
	int lb, ub, period, count;

	if (argc < 7)
	{
		cout << "Call the reader as: ./reader -r lb ub -d period -s shmid\n";
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
			strcpy (memname,argv[i+1]);
		}
	} 	
		
	signal(SIGINT, signal_handler);
	struct shared_data* shared_msg; 

	// open the shared memory object with shm_open() - read, write, execute/search by owner and group 
	shmfd = shm_open(memname, O_RDWR, S_IRWXU | S_IRWXG);
	if (shmfd < 0)
	{
		perror("In shm_open()");
		exit(1);
	}


	// Open semaphores
	access_sem=sem_open("access",  S_IRUSR | S_IWUSR);
	readers_sem=sem_open("readers",  S_IRUSR | S_IWUSR);
	order_sem=sem_open("order", S_IRUSR | S_IWUSR, 1);
	readerscount_sem=sem_open("readers_counter", S_IRUSR | S_IWUSR);
	if (access_sem < 0 || readers_sem < 0 || order_sem < 0 || readerscount_sem < 0)
	{
		perror("In sem_open()");
		int ret = kill(getpid(),SIGINT);
	}

	sem_wait(order_sem);
	sem_wait(readers_sem);
	sem_getvalue(readerscount_sem, &count);
	if (count == 0)
	{
		sem_wait(access_sem);
	}
	sem_post(readerscount_sem);
	sem_post(order_sem);
	sem_post(readers_sem);

	//read

	// mapping the shared memory in the virtual address space of the process
	shared_msg = (struct shared_data*)mmap(NULL, shared_seg_size, PROT_READ, MAP_SHARED, shmfd, 0);
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

	for(int i = lb; i < ub; i++)
	{
		cout<<"PID: "<<shared_msg[i].var1<<"\n";
		cout<<shared_msg[i].var2<<"\n";
		printf("Position modified at %lld.%.9ld seconds\n", (long long)shared_msg[i].var3.tv_sec, shared_msg[i].var3.tv_nsec);
	}

	cout << "Ok now i am going to take a nap\n";
	sleep(period);
	sem_wait(readers_sem);
	sem_wait(readerscount_sem);
	sem_getvalue(readerscount_sem, &count);
	if (count == 0)
	{
		sem_post(access_sem);
	}
	sem_post(readers_sem);

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

}



