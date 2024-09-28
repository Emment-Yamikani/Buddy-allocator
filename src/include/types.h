#pragma once

typedef     char                i8;
typedef     short               i16;
typedef     int                 i32;
typedef     long                i64;
typedef     long                isize;

typedef     unsigned char       uint8_t;
typedef     unsigned short      uint16_t;
typedef     unsigned int        uint32_t;
typedef     unsigned long       uint64_t;

typedef     unsigned char       u8;
typedef     unsigned short      u16;
typedef     unsigned int        u32;
typedef     unsigned long       u64;
typedef     unsigned long       usize;
typedef     unsigned long       size_t;
typedef     unsigned long       uintptr_t;

typedef     float               f32;
typedef     double              f64;

typedef     union  pte_t        pte_t;

typedef     struct spinlock_t   spinlock_t;

typedef     struct cpu_t        cpu_t;
typedef     struct thread_t     thread_t;

typedef     struct inode_t      inode_t;
typedef     struct device_t     device_t;

typedef     struct buddy_t      buddy_t;