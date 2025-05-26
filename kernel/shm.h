// shm.h

#ifndef _SHM_H
#define _SHM_H

#include "types.h"
#include <stddef.h>

#define SHM_RDONLY  0x1000  // Attach read-only
#define SHM_REMAP   0x2000  // Remap existing segment
#define SHM_RDWR    0x3000  // Read-write access

#define IPC_PRIVATE  0
#define IPC_CREAT    0x200
#define IPC_EXCL     0x400


#define IPC_STAT    1  // Get segment information
#define IPC_RMID    2  // Remove shared memory segment
#define IPC_SET     3  // Set segment information

#define SHM_MAX_SEGMENTS 2
#define SHM_SEG_SIZE     PGSIZE

// Shared memory segment structure
struct shmid_ds {
    int shmid;               // Unique segment ID
    int key;                 // Key for the segment
    uint64 size;             // Size of segment
    uint64 addr;             // Attached address
    int pid;               // Creator's PID
    int cpid;              // Last process to attach
    uint mode;             // Permissions
    uint64 nattach;          // Number of attached processes
    uint64 max_nattach;      // Maximum number of attachments
};

// Shared memory segment
struct shm_segment {
    int key;                 // Unique key
    int shmid;               // Segment ID
    void* addr;              // Base address
    uint64 shmaddr;           // Mapped address (to free)
    uint64 size;             // Segment size
    int refcount;            // Reference count
    int flags;               // Flags
    struct proc* owner;      // Owner process
    int toDel;
};


#endif