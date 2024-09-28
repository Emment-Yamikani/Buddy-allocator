#pragma once

#include "defs.h"

#define BUDDY_NORDER    37 // maximum posible orders supported.

#define BUDDY_FREE      0 // Free buddy block.
#define BUDDY_PARTIAL   1 // Partially filled buddy block.
#define BUDDY_FULL      2 // Fully filled buddy block

typedef struct buddy_t {
    buddy_t     *next;  // next block in this order of blocks.
    u64         *bitmap;// bitmap of blocks in this block.
    void        *pages; // array of pages in this block.
    u64         addr;   // base address of block.
    u8          order;  // as an index of the base 2, and order-level.
    u8          state;  // can be FULL, PARTIAL or FREE.
} __packed buddy_t;

#define buddy_addr(b)           ({ (b)->addr; })
#define buddy_size(b)           ({ (1 << (b)->order) * PGSZ; })
#define buddy_end(b)            ({ buddy_addr(b) + buddy_size(b); })

#define BUDDY_LEFT      1
#define BUDDY_RIGHT     2

#define buddy_isbuddy(b, __b1)    ({\
    (buddy_end(b) == buddy_addr(__b1)) ? BUDDY_RIGHT : \
    (buddy_end(__b1) == buddy_addr(b)) ? BUDDY_LEFT : 0;\
})

extern void *buddy_alloc(usize size);
extern void buddy_free(void *ptr);
extern int buddy_init(void);
extern void dump_free_list();
extern void dump_used_list();