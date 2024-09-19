#pragma once

typedef _Atomic volatile unsigned long atomic_t;

#define atomic_inc(p) ({                        \
    __atomic_fetch_add(p, 1, __ATOMIC_SEQ_CST); \
})

#define atomic_dec(p) ({                        \
    __atomic_fetch_sub(p, 1, __ATOMIC_SEQ_CST); \
})

#define atomic_write(p, v) ({                 \
    __atomic_store(p, val, __ATOMIC_SEQ_CST); \
})

#define atomic_read(p) ({               \
    __atomic_load(p, __ATOMIC_SEQ_CST); \
})

#define atomic_test_and_set(p) ({               \
    __atomic_test_and_set(p, __ATOMIC_SEQ_CST); \
})

#define atomic_clear(p) ({               \
    __atomic_clear(p, __ATOMIC_SEQ_CST); \
})
