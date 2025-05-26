#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "shm.h"

// Global shared memory management
struct {
    struct spinlock lock;
    struct shm_segment segments[SHM_MAX_SEGMENTS];
    int isInit;
} shm_table;

//debug
// Add to shm.c
void
print_shm_table(void)
{
    printf("\n--- Shared Memory Table Dump ---\n");
    
    // acquire(&shm_table.lock); //already acquired
    
    printf("Shared Memory Table Status:\n");
    printf("Initialization Status: %s\n", 
           shm_table.isInit ? "Initialized" : "Not Initialized");
    
    printf("\nSegment Details:\n");
    
    
    for (int i = 0; i < SHM_MAX_SEGMENTS; i++) {
        struct shm_segment* seg = &shm_table.segments[i];
        
        // Only print non-empty segments
        // if (seg->key != -1) {
            printf("id:%d key:%d shmid:%d addr:%p size:%ld ref:%d flag:%d toDel:%s\n", 
                   i, 
                   seg->key, 
                   seg->shmid, 
                   seg->addr, 
                   seg->size, 
                   seg->refcount, 
                   seg->flags, 
                   seg->toDel ? "Yes" : "No");
        // }
    }
    
    printf("\nTotal Segments: %d\n", SHM_MAX_SEGMENTS);
    
    // release(&shm_table.lock);
    
    printf("--- End of Shared Memory Table Dump ---\n");
}


// Initialize shared memory system
void
shm_init(void)
{
    static int initialized = 0;
    if (initialized) return ;

    initlock(&shm_table.lock, "shm_table");
    acquire(&(shm_table.lock));
    for (int i = 0; i < SHM_MAX_SEGMENTS; i++) {
        shm_table.segments[i].key = -1;
        shm_table.segments[i].shmid = -1;
        shm_table.segments[i].toDel = 0;
    }
    shm_table.isInit = 1;           //redundant?
    initialized = 1;
    release(&(shm_table.lock));
}

// Find a free shared memory segment
static struct shm_segment*
shm_find_free(void)
{
    for (int i = 0; i < SHM_MAX_SEGMENTS; i++) {
        if (shm_table.segments[i].key == -1) {
            return &shm_table.segments[i];
        }
    }
    
    return NULL;
}

// Find shared memory segment by key
static struct shm_segment*
shm_find_by_key(int key)
{
    for (int i = 0; i < SHM_MAX_SEGMENTS; i++) {
        if (shm_table.segments[i].key == key) {
            return &shm_table.segments[i];
        }
    }
    return NULL;
}

// System call: Create or get shared memory segment
int
sys_shmget(void)
{
    
    if (shm_table.isInit == 0)
    {
      shm_init();
    }

    int key, size, flags;
    struct shm_segment* seg;

    argint(0, &key); 
    argint(1, &size); 
    argint(2, &flags);
    
    acquire(&shm_table.lock);

    // Check if segment already exists
    seg = shm_find_by_key(key);
    if (seg) {
        if (flags & IPC_EXCL) {
            release(&shm_table.lock);
            return -1;  // Segment exists and IPC_EXCL is set
        }
        release(&shm_table.lock);
        return seg->shmid;
    }

    // Create new segment
    if (flags & IPC_CREAT) {
        
        seg = shm_find_free();
        if (!seg) {
            release(&shm_table.lock);
            return -1;  // No free segments
        }
        

        // Initialize segment
        seg->key = key;
        seg->shmid = key;  // Simple mapping for xv6
        seg->size = PGROUNDUP(size > 0 ? size : PGSIZE);
        seg->addr = kalloc();  // Allocate memory
        seg->shmaddr = (uint64)seg->addr;
        seg->refcount = 0;
        seg->flags = flags;
        seg->owner = myproc();

        memset(seg->addr, 0, seg->size);

        release(&shm_table.lock);
        return seg->shmid;
    }

    release(&shm_table.lock);
    return -1;
}

// System call: Attach shared memory segment
int
sys_shmat(void)
{
    // printf("shmat:trying to attach\n");
   

    int shmid, shmflg;
    uint64 shmaddr;
    struct shm_segment* seg;
    struct proc* p = myproc();

    argint(0, &shmid); 
    argaddr(1, &shmaddr); 
    argint(2, &shmflg);

    if (shmaddr == 0) 
    {
      // Find a suitable unmapped address
      shmaddr = p->sz;  // Start from the current process size    // becomes VA, so use something else????
    }
    else if ((shmaddr & (PGSIZE-1)) != 0) 
    {
        printf("shmat:alignment failure\n");
        return -1;  // Address must be page-aligned
    }

    acquire(&shm_table.lock);

    // Find segment
    seg = shm_find_by_key(shmid);
    if (!seg) {
        printf("no seg found\n");
        release(&shm_table.lock);
        return -1;
    }

    myproc()->sz += PGROUNDUP(seg->size);
    // Attach segment to process
    if (mappages(p->pagetable, shmaddr, PGSIZE, 
                 (uint64)seg->addr, 
                 PTE_R | PTE_W | PTE_U) < 0) 
    {
        // printf("shmat:mapping failed\n");
        myproc()->sz -= PGROUNDUP(seg->size);
        release(&shm_table.lock);
        return -1;
    }
    seg->shmaddr = shmaddr;
    seg->refcount++;
    
    release(&shm_table.lock);
    return (int)shmaddr;
}

// System call: Detach shared memory segment
int
sys_shmdt(void)
{
    uint64 shmaddr;
    struct proc* p = myproc();
    struct shm_segment* seg = NULL;

    argaddr(0, &shmaddr);

    acquire(&shm_table.lock);

    // Find segment by mapped address
    for (int i = 0; i < SHM_MAX_SEGMENTS; i++) {
        if (shm_table.segments[i].shmaddr == shmaddr) {
            seg = &shm_table.segments[i];
            break;
        }
    }

    if (!seg) {
        printf("No seg\n");
        release(&shm_table.lock);
        return -1;
    }

    // debug
    printf("Detaching segment: key %d, addr %p, size %ld, refcount %d\n", 
           seg->key, seg->addr, seg->size, seg->refcount);

    if (seg->refcount == 0)
    {
        release(&shm_table.lock);
        return -1;
    }    
    // Unmap from process
    
    // uvmunmap(p->pagetable, shmaddr, PGROUNDUP(seg->size) / PGSIZE, 1);
    // p->sz -= PGROUNDUP(seg->size) / PGSIZE ; //seg->size;
    p->sz = p->sz;
    

    // Decrement reference count
    seg->refcount--;

    // Free if no more references
    if (seg->refcount <= 0 && seg->toDel == 1) {
        printf("Freeing shared memory segment\n");
        kfree(seg->addr);
        // Clear the mapped address
        seg->shmaddr = 0;
        seg->addr = NULL;
        seg->key = -1;
        seg->shmid = -1;
        seg->size = 0;
        seg->flags = 0;
        seg->owner = 0;
        seg->toDel = 0;
        
    }
    
    release(&shm_table.lock);
    
    return 0;
}

// System call: Control shared memory segment
int
sys_shmctl(void)
{
    int shmid, cmd;
    struct shmid_ds* buf;

    argint(0, &shmid);
    argint(1, &cmd);
    argaddr(2, (uint64*)&buf);
    
    acquire(&shm_table.lock);
    
    struct shm_segment* seg = shm_find_by_key(shmid);
    
    if (!seg) {
        release(&shm_table.lock);
        return -1;
    }
    

    switch(cmd) {
        case IPC_STAT:  // Get segment information
            if (copyout(myproc()->pagetable, (uint64)buf, (char*)seg, sizeof(struct shmid_ds)) < 0) {
                release(&shm_table.lock);
                return -1;
            }
            break;

        case IPC_RMID:  // Remove shared memory segment
            
            //acquire(&shm_table.lock);   //panic acquire
            if (seg->key != -1)
                seg->toDel = 1;
            // if (seg->key != -1) {
            //     seg->toDel = 1;
            //     if (seg->refcount <= 0) {

            //         uvmunmap(myproc()->pagetable, seg->shmaddr, PGROUNDUP(seg->size) / PGSIZE, 1);
            //         myproc()->sz -= seg->size;
            //         if (seg->addr) {
            //             kfree(seg->addr);
            //             seg->addr = 0;
            //         }
            //         memset(seg, 0, sizeof(struct shm_segment));
            //         seg->key = -1;
            //         seg->shmid = -1;
            //         seg->toDel = 0;
                    
            //     }
            // }
            // release(&shm_table.lock);
            break;

        case IPC_SET:  // Set segment information (not implemented)
            // TODO
            release(&shm_table.lock);
            return -1;  // Not implemented

        default:
            release(&shm_table.lock);
            return -1;  // Invalid command
    }

    release(&shm_table.lock);
    return 0;
}