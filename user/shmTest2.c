// shm_test2.c

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/shm.h"

#define SHM_KEY 12345
#define SHM_SIZE 4096



void test_multiple_attach() {
    printf("\nTest: Multiple Attachment\n");


    int pid = fork();                             
    if (pid < 0) {
        printf("FAIL: fork failed\n");
        exit(1);
    }

    if (pid == 0) {  // Child process
        int shmid = shmget(SHM_KEY, 0, IPC_CREAT);
        if (shmid < 0) {
            printf("FAIL: shmget failed\n");
            exit(1);
        }

        char* shared_mem = (char*)shmat(shmid, 0, 0);
        if (shared_mem == (char*)-1) {
            printf("CHILD: FAIL: shmat failed\n");
            exit(1);
        }
        printf("CHILD: Attached shared memory\n");
        // Read from shared memory
        printf("CHILD: Read from shared memory: %s\n", shared_mem);
        
        if (shmdt(shared_mem) < 0) {
            printf("CHILD: FAIL: shmdt failed\n");
            exit(1);
        }
        printf("CHILD: Detached shared memory\n");
        exit(0);
    } else {  // Parent process
        int shmid = shmget(SHM_KEY, 0, IPC_CREAT);
        if (shmid < 0) {
            printf("FAIL: shmget failed\n");
            exit(1);
        }
        char* shared_mem = (char*)shmat(shmid, 0, 0);
        if (shared_mem == (char*)-1) {
            printf("PARENT: FAIL: shmat failed\n");
            exit(1);
        }
        printf("PARENT: Attached shared memory\n");

        // Write to shared memory
        strcpy(shared_mem, "Shared data between parent and child");
        printf("PARENT: Wrote to shared memory\n");

        // Wait for child
        wait(0);

        //parent detach

        if (shmdt(shared_mem) < 0) {
            printf("PARENT: FAIL: shmdt failed\n");
            exit(1);
        }
        printf("PARENT: Detached shared memory\n");

        if (shmctl(shmid, IPC_RMID, 0) < 0) {
            printf("PARENT: FAIL: shmctl IPC_RMID failed\n");
            exit(1);
        }
        printf("PARENT: Removed shared memory segment\n");
    }
}

int main() {
    test_multiple_attach();  // Call the test function

    printf("\nAll tests passed!\n");
    exit(0);
}
