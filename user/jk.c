#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include <stddef.h>
#include <user/wait.h>

int main() {
    int pid = fork();
    if (pid == 0) {
        // Child process
        sleep(1);
        printf("Child process with PID: %d exiting with status 42\n", getpid());
        exit(42);  // Exit with status 42
    } else {
        // Parent process
        // waitpid(pid, &status);  // Wait for the child process
        int status;
        int n = waitpid(pid,&status,0);
        if (n == 0)
        {
            printf("WNOHANG TEST\n");
        }
        printf("Child exited with status:%d\n", status);
    }
    exit(0);
}

