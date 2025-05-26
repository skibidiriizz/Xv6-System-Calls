#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_waitpid(void) {
    int pid;
    uint64 status_addr;
    int options;

    argint(0, &pid);
    argaddr(1, &status_addr);
    argint(2, &options);
    
    return waitpid(pid, status_addr, options);
}

uint64
sys_sigraise(void)
{
  int pid;
  int SIG;
  argint(0, &pid);
  argint(1, &SIG);
  return sigraise(pid,SIG); 
}

uint64 sys_settickets(void) {
    int tickets;
    int pid;

    // Retrieve the arguments from user space
    argint(0, &pid);
    argint(1, &tickets);
    

    // Validate the ticket count
    if (tickets < 1) {
        return -1; // Invalid ticket count
    }

    return settickets(pid,tickets);
}

#include "semaphore2.h"
uint64 sys_sem2init(void) {

    uint64 addr;
    int icount;
    argaddr(0, &addr);
    argint(1,&icount);

    sem2init(addr, icount);
    return 0; // Success

}

uint64 sys_sem2_wait(void) {

    uint64 sem_id;
    argaddr(0, &sem_id);
    sem2_wait(sem_id);
    return 0; // Success

}

uint64 sys_sem2_post(void) {

    uint64 sem_id;
    argaddr(0, &sem_id);
    sem2_post(sem_id);
    return 0; // Success

}


