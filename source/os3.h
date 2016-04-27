#include <time.h>

struct shared_data {
    int var1;
    char var2[100];
    timespec var3;
};

sem_t * access_sem;
sem_t * readers_sem;
sem_t * order_sem;
sem_t * readerscount_sem;
