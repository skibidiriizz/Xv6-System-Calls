#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#include <stddef.h>

#define NUM_PROCESSES 5

void child_process(int pid, int tickets) {
    int count = 0;

    // Set the number of tickets for this process
    settickets(pid, tickets);

    // Loop to simulate work and yield to the scheduler
    for (int i = 0; i < 10; i++) {
        printf("Process %d with %d tickets running: %d\n", pid, tickets, count);
        count++;
        sleep(10); // Sleep for a short period to yield to the scheduler
    }
    exit(0);
}

int main(int argc, char *argv[]) {
    int pids[NUM_PROCESSES];
    int tickets[NUM_PROCESSES] = {1, 2, 3, 4, 5}; // Different ticket values

    // Create child processes
    for (int i = 0; i < NUM_PROCESSES; i++) {
        if ((pids[i] = fork()) == 0) {
            // Child process
            child_process(getpid(), tickets[i]);
        }
    }

    // Parent process waits for all child processes to finish
    for (int i = 0; i < NUM_PROCESSES; i++) {
        wait(NULL);
    }

    printf("All child processes have finished.\n");
    exit(0);
}