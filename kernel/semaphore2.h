#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include "spinlock.h"  // Include the actual spinlock definition
#include "types.h"     // For uint type

// Forward declaration for proc
struct proc;

struct semaphore2 {
  struct spinlock lock;  // Protects count and waiters
  int count;            // Number of resources available
  struct {
    struct proc *proc;  // Process waiting on semaphore
    int valid;         // Is this slot valid?
  } waiters[64];       // Array of waiting processes
};



void sem2init(uint64 addr, int initial_count);
void sem2_wait(uint64 addr);
void sem2_post(uint64 addr);

#endif // SEMAPHORE_H