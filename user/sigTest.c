#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include <stddef.h>
#include "kernel/sig.h"



int main ()
{
    int pid;

    // Step 1: Create a new process (e.g., a dummy process or a forked process)
    pid = fork();
    if(pid == 0) 
    {
        // Child process does nothing, just sleeps in the kernel.
        while(1) {
            sleep(10); 
        }
    }
    else
    {
        printf("SIG STOP:%d\n",SIG_STOP);
        sigraise(pid, SIG_STOP);
        
        printf("SIG CONT:%d\n", SIG_CONT);
        sigraise(pid, SIG_CONT);

        printf("SIG TERM:%d\n", SIG_TERM);
        sigraise(pid, SIG_TERM);

        printf("SIG KILL:%d\n", SIG_KILL);
        sigraise(pid, SIG_KILL);
    }
    printf("DONE\n");
    exit(0);
}

