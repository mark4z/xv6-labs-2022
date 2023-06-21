// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

struct {
    struct spinlock lock;
    struct buf buf[NBUF];
    struct bucket_entry bucket[HASH];
} bcache;

void
binit(void) {
    struct buf *b;
    struct bucket_entry *bucket;
    char *name[6];

    initlock(&bcache.lock, "bcache");
    // Create linked list of buffers
    for (int i = 0; i < HASH; ++i) {
        bucket = &bcache.bucket[i];
        snprintf((char *) name, 8, "kmem-%d", i);
        initlock(&bucket->lock, (char *) name);
    }

    // Create linked list of buffers
    bucket = &bcache.bucket[0];
    for (b = bcache.buf; b < bcache.buf + NBUF; b++) {
        initsleeplock(&b->lock, "buffer");
        b->next = bucket->head.next;
        bucket->head.next = b;
    }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf *
bget(uint dev, uint blockno) {
    struct buf *b;
    struct buf *pre;
    uint idx = blockno % HASH;
    struct bucket_entry *bucket = &bcache.bucket[idx];

    acquire(&bucket->lock);
    // Is the block already cached?
    for (b = bucket->head.next; b; b = b->next) {
        if (b->dev == dev && b->blockno == blockno) {
            b->refcnt++;
            b->ticks = ticks;
            release(&bucket->lock);
            acquiresleep(&b->lock);
            return b;
        }
    }
    release(&bucket->lock);

    acquire(&bcache.lock);
    // Not cached.
    for (int i = 0; i < HASH; ++i) {
        struct bucket_entry *borrow_bucket = &bcache.bucket[i];

        acquire(&borrow_bucket->lock);

        pre = &borrow_bucket->head;
        for (b = pre->next; b ; b = b->next) {
            if (b->refcnt == 0) {
                pre->next = b->next;
                b->next = 0;

                b->dev = dev;
                b->blockno = blockno;
                b->valid = 0;
                b->refcnt = 1;
                release(&borrow_bucket->lock);

                acquire(&bucket->lock);

                b->next = bucket->head.next;
                bucket->head.next = b;

                release(&bucket->lock);
                release(&bcache.lock);

                acquiresleep(&b->lock);
//                printf("allocate: %p\n", b);
                return b;
            }
            pre = b;
        }
        release(&borrow_bucket->lock);
    }
    panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf *
bread(uint dev, uint blockno) {
    struct buf *b;

    b = bget(dev, blockno);
    if (!b->valid) {
        virtio_disk_rw(b, 0);
        b->valid = 1;
    }
    return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b) {
    if (!holdingsleep(&b->lock))
        panic("bwrite");
    virtio_disk_rw(b, 1);
}

// Release a locked buffer.
// Move to the head of the most-recently-used list.
void
brelse(struct buf *b) {
    if (!holdingsleep(&b->lock))
        panic("brelse");

    releasesleep(&b->lock);

    uint idx = b->blockno % HASH;
    struct bucket_entry *bucket = &bcache.bucket[idx];

    acquire(&bucket->lock);
    b->refcnt--;
    release(&bucket->lock);
}

void
bpin(struct buf *b) {
    acquire(&bcache.lock);
    b->refcnt++;
    release(&bcache.lock);
}

void
bunpin(struct buf *b) {
    acquire(&bcache.lock);
    b->refcnt--;
    release(&bcache.lock);
}


