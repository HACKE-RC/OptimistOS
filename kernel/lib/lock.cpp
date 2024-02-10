#include <cstddef>
#include <cstdint>
#include "lock.hpp"

void lock(uint32_t &lock) {
    while (__sync_lock_test_and_set(&lock, 1)) {
        ;;
    }
}

void unlock(uint32_t &lock) {
    __sync_lock_release(&lock);
}