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
	

	if (argc != 2)
	{
		cout << "Call the init process as: ./init [memory name]\n";
		exit(-1);
	}
	strcpy (memname,argv[1]);

	// Shared segment unlink: Remove a named posix shared segment from the system.
	if (shm_unlink(memname) != 0) 
	{
		perror("In shm_unlink()");
		//exit(1);
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
	cout<< "Cleaning Process: Evething has been cleaned from the system\n";
}
