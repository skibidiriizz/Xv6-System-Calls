#include "kernel/types.h"
#include "kernel/stat.h"
// #include "kernel/spinlock.h"
#include "user/user.h"
#include "kernel/shm.h" 
#include "kernel/semaphore2.h" 

#define SHM_KEY 12345
#define SHM_SIZE 4096
#define NUM_PROCESSES 2


int main(int argc, char *argv[]) {
    
    int shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT);
    struct semaphore2 *lock = (struct semaphore2 *)shmat(shmid, 0, 0);
    if (lock == (void*)-1) {
        printf("FAIL: shmat failed\n");
        exit(1);
    }
    
    sem2init((uint64)lock, 1); // Initialize semaphore


        if (fork() == 0) { // Child process

            sem2_wait((uint64)lock);
            // sleep(10); 
            // printf("child %d \n", getpid());
            write(1, "CHILD\n", strlen("CHILD\n"));
            __sync_synchronize();
            sem2_post((uint64)lock); // Release semaphore
            
            exit(0); // Exit child process

        }
        else
        {
            sem2_wait((uint64)lock); 
            // printf("Parent %d \n", getpid());
            write(1, "PARENT\n", strlen("PARENT\n"));
            __sync_synchronize();
            sem2_post((uint64)lock); // Release semaphore
            shmdt(lock);
            exit(0); // Exit parent process

        }


    
    exit(0);
}

// int main(int argc, char *argv[]) {
    
//     struct semaphore2 lock;
//     sem2init((uint64)&lock, 1); // Initialize semaphore with value 1 (binary semaphore)

    
//     sem2_wait((uint64)&lock);
//     printf("semaphore testing\n");
//     sem2_post((uint64)&lock);


//     printf("All child processes have finished.\n");
//     exit(0);
// }