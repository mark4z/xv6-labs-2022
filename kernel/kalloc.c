// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end, int cpu);

void kfree_inner(void *pa, int cpuid);

extern char end[]; // first address after kernel.
// defined by kernel.ld.

struct run {
    struct run *next;
};

struct {
    struct spinlock lock;
    struct run *freelist;
} kmem[NCPU];

void
kinit() {
    uint64 start = PGROUNDUP((uint64) end);
    uint64 avg = (PHYSTOP - start) / PGSIZE / NCPU;
    printf("%p %d %p %p\n", start, avg, start + avg * NCPU * PGSIZE, PHYSTOP);

    char *buf[6];
    for (int i = 0; i < NCPU; ++i) {
        snprintf((char *) buf, 6, "kmem-%d", i);
        initlock(&kmem[i].lock, (char *) buf);
        char *l = end + (i * avg) * PGSIZE;
        char *r = (char *) l + avg * PGSIZE;
        if (i == NCPU - 1) {
            r = (char *) PHYSTOP;
        }

        printf("cpu %d: %p %p\n", i, l, r);
        freerange(l, r, i);
    }
}

void
freerange(void *pa_start, void *pa_end, int cpuid) {
    char *p;
    p = (char *) PGROUNDUP((uint64) pa_start);
    for (; p + PGSIZE <= (char *) pa_end; p += PGSIZE)
        kfree_inner(p, cpuid);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree_inner(void *pa, int cpuid) {
    struct run *r;

    if (((uint64) pa % PGSIZE) != 0 || (char *) pa < end || (uint64) pa >= PHYSTOP)
        panic("kfree");

    // Fill with junk to catch dangling refs.
    memset(pa, 1, PGSIZE);

    r = (struct run *) pa;

    acquire(&kmem[cpuid].lock);
    r->next = kmem[cpuid].freelist;
    kmem[cpuid].freelist = r;
    release(&kmem[cpuid].lock);
}

void kfree(void *pa) {
    push_off();
    int id = cpuid();
    pop_off();
    kfree_inner(pa, id);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void) {
    push_off();
    int id = cpuid();
    pop_off();

    struct run *r;

    acquire(&kmem[id].lock);
    r = kmem[id].freelist;
    if (r) {
        kmem[id].freelist = r->next;
        release(&kmem[id].lock);
    } else {
        release(&kmem[id].lock);

        for (int i = 0; i < NCPU; ++i) {
            acquire(&kmem[i].lock);
            struct run *or = kmem[i].freelist;
            if (or) {
                r = or;
                kmem[i].freelist = or->next;
                release(&kmem[i].lock);
                break;
            }
            release(&kmem[i].lock);
        }
    }
    if (r)
        memset((char *) r, 5, PGSIZE); // fill with junk
    return (void *) r;
}
