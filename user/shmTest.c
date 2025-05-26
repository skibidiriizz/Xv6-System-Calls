// shm_test.c

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/shm.h"

#define SHM_KEY 12345
#define SHM_SIZE 4096

void test_basic_shm() {
    
    printf("Test: Basic Shared Memory\n");

    // Create shared memory segment
    int shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT);
    if (shmid < 0) {
        printf("FAIL: shmget failed\n");
        exit(1);
    }
    printf("shmget successful, shmid: %d\n", shmid);

    // Attach shared memory
    // In shmTest.c
    char* shared_mem = (char*)shmat(shmid, 0, 0);
    if (shared_mem == (void*)-1) {  // Use (void*)-1 instead of (char*)-1
        printf("FAIL: shmat failed\n");
        exit(1);
    }
    printf("shmat successful\n");

    // Write to shared memory
    strcpy(shared_mem, "Hello, Shared Memory!");
    printf("Wrote to shared memory: %s\n", shared_mem);

    

    // Remove shared memory segment
    if (shmctl(shmid, IPC_RMID, 0) < 0) {
        printf("FAIL: shmctl IPC_RMID failed\n");
        exit(1);
    }
    printf("shmctl IPC_RMID successful\n");

    // Detach shared memory
    if (shmdt(shared_mem) < 0) {
        printf("FAIL: shmdt failed\n");
        exit(1);
    }
    printf("shmdt successful\n");
    

}



void test_duplicate_create() {
    printf("\nTest: Duplicate Shared Memory Creation\n");

    // First creation
    int shmid1 = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT);
    if (shmid1 < 0) {
        printf("FAIL: First shmget failed\n");
        exit(1);
    }
    printf("First shmget successful, shmid: %d\n", shmid1);

    // Try to create again with IPC_EXCL
    int shmid2 = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | IPC_EXCL);
    if (shmid2 >= 0) {
        printf("FAIL: Second shmget with IPC_EXCL should have failed\n");
        exit(1);
    }
    printf("Second shmget with IPC_EXCL correctly failed\n");

    // Clean up
    if (shmctl(shmid1, IPC_RMID, 0) < 0) {
        printf("FAIL: shmctl IPC_RMID failed\n");
        exit(1);
    }
    printf("Removed shared memory segment\n");
}

int main() {
    
    test_basic_shm();   // f1 
    // printf("Next\n");
    
    test_duplicate_create(); //f3

    printf("\nAll shared memory tests passed!\n");
    exit(0);
}