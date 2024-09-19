#pragma once

#include "types.h"

#define __unused                    __attribute__((unused))
#define __packed                    __attribute__((packed))
#define __noreturn                  __attribute__((noreturn))
#define __aligned(n)                __attribute__((aligned(n)))
#define __fallthrough               __attribute__((fallthrough))
#define __section(s)                __attribute__((section(#s)))
#define __aligned_section(s, a)     __attribute__((section(#s), aligned(a)))
#define __used_section(__section__) __attribute__((used, section(#__section__)))

#define __CAT(a, b)                 a##b

#ifndef nullptr
#define nullptr     ((void *)0)
#endif

#ifndef NULL
#define NULL        nullptr
#endif

#define loop()      for (;;)

#ifndef forlinked
#define forlinked(elem, list, iter) \
    for (typeof(list) elem = list; elem; elem = iter)
#endif  // forlinked

#ifndef foreach
#define foreach(elem, list)                                             \
    for (typeof(*list) *__tmp__foreach = list,                          \
        elem = (typeof(elem))(__tmp__foreach ? *__tmp__foreach : NULL); \
        elem; elem = *++__tmp__foreach)
#endif  // foreach

#ifndef foreach_reverse
#define foreach_reverse(elem, list)                                     \
    for (typeof(*list) *__tmp__foreach = list,                          \
        elem = (typeof(elem))(__tmp__foreach ? *__tmp__foreach : NULL); \
        elem; elem = *--__tmp__foreach)
#endif  // foreach_reverse

#ifndef container_of
#define container_of(ptr, type, member) ({             \
    const typeof(((type *)0)->member) *__mptr = (ptr); \
    (type *)((char *)__mptr - offsetof(type, member)); \
})
#endif

#define KiB(x)          ((usize)(x) * 1024ull)         // convert from KiB to bytes.
#define MiB(x)          ((usize)(x) * KiB(1024ull))    // convert from MiB to bytes.
#define GiB(x)          ((usize)(x) * MiB(1024ull))    // convert from GiB to bytes.

#define B2KiB(x)        ((usize)(x) / KiB(1))   // convert bytes to KiB.
#define B2MiB(x)        ((usize)(x) / MiB(1))   // convert bytes to MiB.
#define B2GiB(x)        ((usize)(x) / GiB(1))   // convert bytes to GiB.

#define K2MiB(x)        ((usize)(x) / KiB(1))   // convert kiB from to MiB.
#define K2GiB(x)        ((usize)(x) / MiB(1))   // convert kiB from to GiB.

#define M2KiB(x)        ((usize)(x) * KiB(1))   // convert MiB from to KiB.
#define M2GiB(x)        ((usize)(x) / KiB(1))   // convert MiB from to GiB.

#define G2KiB(x)        ((usize)(x) * MiB(1))   // convert GiB from to KiB.
#define G2MiB(x)        ((usize)(x) * KiB(1))   // convert GiB from to MiB.

// page size used by gingerOs
#define PGSZ            (0x1000ull)
// page size used for a 2MiB page
#define PGSZ2MB         (MiB(2))
#define PGSZ1GB         (GiB(1))
// page mask
#define PGMSK           (PGSZ - 1)
#define PGOFF(p)        ((uintptr_t)(p) & PGMSK)
// Round off to the lowest page-aligned address.
#define PGROUND(p)      ((uintptr_t)(p) & ~PGMSK)
// Round up(is not page-aligned add 1 page and round to lowest page-aligned) an address.
#define PGROUNDUP(p)    (PGOFF(p) ? PGROUND((uintptr_t)(p) + PGSZ): (uintptr_t)(p))

// page-align an address.
#define ALIGNPG(p)      (PGROUNDUP(p))
// Page count from size, if x is not page aligned '1' page is added.
#define NPAGE(x)        (((usize)(x) / PGSZ) + (PGOFF(x) ? 1 : 0))

// # of elements in an array.
#define NELEM(x)        (sizeof((x)) / sizeof ((x)[0]))

// magic return address of user threads.
#define MAG_RET         (-1ull)

// Base address of the kernel
#define VMABASE         (0xffff800000000000ull)  
// convert a higher-half virtual address to a lower-half virtual address.
#define VMA2LO(p)       ((uintptr_t)(p) - VMABASE)
// convert a lower-half virtual address to a higher-half virtual address.
#define VMA2HI(p)       ((uintptr_t)(p) + VMABASE)

#define V2LO(p)     ((uintptr_t)(p) - VMABASE)
#define V2HI(p)     ((uintptr_t)(p) + VMABASE)

#define NOT(a)      (~(uintptr_t)(a))
#define BS(x)       ((uintptr_t)(1) << (x))
#define AND(a, b)   ((uintptr_t)(a) & (uintptr_t)(b))
#define OR(a, b)    ((uintptr_t)(a) | (uintptr_t)(b))
#define XOR(a, b)   ((uintptr_t)(a) ^ (uintptr_t)(b))
#define SHL(a, b)   ((uintptr_t)(a) <<(uintptr_t)(b))
#define SHR(a, b)   ((uintptr_t)(a) >>(uintptr_t)(b))
#define NAND(a, b)  (AND(NOT((a)), (b)))
#define BTEST(a, b) (AND((a), BS(b)))
#define MAX(a, b)   ((long)((a) > (b) ? (a) : (b)))
#define MIN(a, b)   ((long)((a) < (b) ? (a) : (b)))
#define ABS(a)      ((long)(a) < (0) ? -(long)(a) : (a))