#include "include/buddy.h"
#include "include/spinlock.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

// Static variables for memory pool, free list, and used list
static usize   memsize = 0;
static buddy_t *pool = NULL;
static buddy_t *free_list[BUDDY_NORDER] = {NULL};
static buddy_t *used_list[BUDDY_NORDER] = {NULL};

// Helper macros for readability
#define ALIGN_UP(x, align) (((x) + ((align)-1)) & ~((align)-1))

// Utility function to dump information about a buddy block
void dump_buddy(buddy_t *block) {
    assert(block, "No buddy block\n");
    printf("block: %p, addr: %16p, order: %2d, next: %p, state: %1d\n",
           block, (void *)block->addr, block->order, block->next, block->state);
}

// Get the order of a block size (log2 of size/PGSZ)
static int get_order(usize size) {
    int order = 0;
    size = ALIGN_UP(size, PGSZ); // Ensure the size is aligned to page size
    while (size > PGSZ) {
        size >>= 1;
        order++;
    }
    return order;
}

// Initialize a buddy block to the FREE state
static void block_init(buddy_t *block) {
    memset(block, 0, sizeof(*block));
    block->state = BUDDY_FREE;
}

// Get a block from the pool of available blocks
static int get_pool(buddy_t **ref) {
    if (!ref) return -EINVAL;

    buddy_t *block = pool;
    if (block) {
        pool = block->next;
        block->next = NULL;
        block_init(block);
        *ref = block;
        return 0;
    }
    return -ENOMEM;
}

// Return a block back to the pool
static int put_pool(buddy_t *block) {
    if (!block) return -EINVAL;
    block->next = pool;
    pool = block;
    return 0;
}

// Add a block to the free list
static int put_free(buddy_t *block) {
    if (!block) return -EINVAL;
    block->next = free_list[block->order];
    free_list[block->order] = block;
    return 0;
}

// Add a block to the used list
static int put_used(buddy_t *block) {
    if (!block) return -EINVAL;
    block->next = used_list[block->order];
    used_list[block->order] = block;
    return 0;
}

// Find and remove a block from the used list by its address
static int find_used(uintptr_t addr, buddy_t **ref) {
    if (!ref) return -EINVAL;
    
    for (int i = 0; i < BUDDY_NORDER; ++i) {
        buddy_t *block = used_list[i];
        buddy_t *prev  = NULL;

        while (block) {
            if (block->addr == addr) {
                if (prev) {
                    prev->next = block->next;
                } else {
                    used_list[i] = block->next;
                }
                block->next = NULL;
                *ref = block;
                return 0;
            }
            prev = block;
            block = block->next;
        }
    }
    return -ENOENT;
}

// Split a block into two buddies of the next lower order
static int split_block(buddy_t *block) {
    if (!block) return -EINVAL;

    buddy_t *buddy = NULL;
    int err = get_pool(&buddy);
    if (err) return err;

    block->order -= 1;
    buddy->order = block->order;
    buddy->addr = block->addr + ((1 << block->order) * PGSZ);

    // Insert buddy into free list
    return put_free(buddy);
}

// Get a free block of a specific order
static int get_free(int order, buddy_t **ref) {
    buddy_t *block = NULL;

    if (order >= BUDDY_NORDER || !ref) return -EINVAL;

    // Search for a block in the free list of the required order
    for (int i = order; i < BUDDY_NORDER; ++i) {
        block = free_list[i];
        if (block) {
            free_list[i] = block->next;

            // Split the block until we reach the required order
            while (block->order > order) {
                int err = split_block(block);
                if (err) return err;
            }

            block->next = NULL;
            block->state = BUDDY_FULL;
            *ref = block;
            return put_used(block);
        }
    }
    return -ENOMEM;
}

// Allocate memory using buddy system
void *buddy_alloc(usize size) {
    buddy_t *block = NULL;
    int err = get_free(get_order(size), &block);
    if (err) return NULL;
    return (block ? (void *)block->addr : NULL);
}

// Free a block and merge with its buddy if possible
void buddy_free(void *ptr) {
    buddy_t *block = NULL;
    int err = find_used((uintptr_t)ptr, &block);
    if (err) {
        panic("Failed to find the block at %p\n", ptr);
        return;
    }

    // Try to find the buddy and merge if possible
    while (block->order < BUDDY_NORDER - 1) {
        buddy_t *buddy = free_list[block->order];
        buddy_t *prev_buddy = NULL;

        while (buddy) {
            int where = buddy_isbuddy(block, buddy);
            if (where == BUDDY_RIGHT || where == BUDDY_LEFT) {
                // Merge the blocks
                if (where == BUDDY_LEFT) {
                    block->addr = buddy->addr;
                }
                block->order += 1;

                // Remove buddy from free list
                if (prev_buddy) {
                    prev_buddy->next = buddy->next;
                } else {
                    free_list[block->order - 1] = buddy->next;
                }

                // Return buddy to the pool
                put_pool(buddy);
                
                // Continue checking for more buddies
                buddy = free_list[block->order];
                prev_buddy = NULL;
                continue;
            }

            prev_buddy = buddy;
            buddy = buddy->next;
        }
        break;
    }

    // Mark the block as free and reinsert it into the free list
    block->state = BUDDY_FREE;
    put_free(block);
}

void dump_free_list() {
    for (int i = 0; i < BUDDY_NORDER; ++i) {
        printf("Free list order %d:\n", i);
        buddy_t *block = free_list[i];
        while (block) {
            dump_buddy(block);
            block = block->next;
        }
    }
}

void dump_used_list() {
    for (int i = 0; i < BUDDY_NORDER; ++i) {
        printf("Used list order %d:\n", i);
        buddy_t *block = used_list[i];
        while (block) {
            dump_buddy(block);
            block = block->next;
        }
    }
}

// Initialize buddy allocator
int buddy_init(void) {
    memsize = KiB(32); // 32KB memory for the buddy system

    memset(free_list, 0, sizeof(free_list));
    memset(used_list, 0, sizeof(used_list));

    // Initialize memory pool with buddy blocks
    pool = (buddy_t *)calloc(NPAGE(memsize), sizeof(buddy_t));
    if (!pool) return -ENOMEM;

    for (usize i = 0; i < NPAGE(memsize); ++i) {
        put_pool(&pool[i]);
    }

    // Initialize the first large block
    buddy_t *block = NULL;
    int err = get_pool(&block);
    if (err) return err;

    block->addr = 0;
    block->order = get_order(memsize);
    block->state = BUDDY_FREE;

    return put_free(block);
}