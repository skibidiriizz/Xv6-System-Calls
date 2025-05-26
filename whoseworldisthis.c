#include "types.h"
#include "stat.h"
#include "user.h"

int main() {
    int pid = myFork(5);  // Create a child process with priority 5
    if(pid == 0) {
        printf(1, "Child process created with priority 5.\n");
    } else {
        printf(1, "Parent created child process with PID %d.\n", pid);
    }
    exit();
}
