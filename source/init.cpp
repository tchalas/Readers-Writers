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
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include "os3.h"
using namespace std;

//call this function for classic shared mem implemantion
/*
int getmem( key_t keyval, int segmentsize )
{
        int shmid;
        
        if((shmid = shmget( keyval, segmentsize, IPC_CREAT | 0660 )) == -1)
        {
                return(-1);
        }
        
        return(shmid);
}*/

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
	int shared_seg_size = (50 * sizeof(struct shared_data)); /* want shared segment capable of storing 1 message */
	timespec ts;
	struct shared_data* shared_msg; /* the shared segment, and head of the messages list */
	
	signal(SIGINT, signal_handler);

	if (argc != 2)
	{
		cout << "Call the init process as: ./init [memory name]\n";
		exit(-1);
	}
	strcpy (memname,argv[1]);

	/*
	int sg = getmem( key_t keyval, shared_seg_size );
	shared_msg = shmat(shared.shmid, (void *) 0, 0);
	/* Detach segment 
	err = shmdt((void *) shared_msg);
	if (err == -1) perror ("Detachment.");
	*/


	// create the shared memory object with shm_open() - read, write, execute/search by owner and group 
	shmfd = shm_open(memname, O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
	if (shmfd < 0)
	{
		perror("In shm_open()");
		exit(1);
	}

    	printf("Created shared memory object %s\n", memname);

	// adjusting mapped file size (make room for the whole segment to map) with ftruncate() 
    	int f = ftruncate(shmfd, shared_seg_size);
	if (f < 0)
	{
		if ( shm_unlink(memname) < 0 )
		{
		 perror("shm_unlink");
		}
		perror("In ftruncate");
		exit(1);
	}

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
	printf("Shared memory segment allocated correctly (%d bytes).\n", shared_seg_size);

	//initialize values in shared memory segment
	for (int i = 0; i < 50; i++)
	{
		shared_msg[i].var1 = -1;
		sprintf(shared_msg[i].var2,"Nothing written here my friend. The writers are very lazy! Come back later!");
		clock_gettime(CLOCK_REALTIME, &ts);
		shared_msg[i].var3 = ts;
	}

	// create and initialize all the semaphores
	access_sem=sem_open("access", O_CREAT, S_IRUSR | S_IWUSR, 1);
	readers_sem=sem_open("readers", O_CREAT, S_IRUSR | S_IWUSR, 1);
	order_sem=sem_open("order", O_CREAT, S_IRUSR | S_IWUSR, 1);
	readerscount_sem=sem_open("readers_counter", O_CREAT, S_IRUSR | S_IWUSR, 0);
	if (access_sem < 0 || readers_sem < 0 || order_sem < 0 || readerscount_sem < 0)
	{
		perror("In sem_open()");
		int ret = kill(getpid(),SIGINT);
	}
	cout << "Init Process: All semaphores are created\n";
	cout << "Init Process: Writers - Readers enviroment has been initialized\n";

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












