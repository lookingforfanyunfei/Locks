//
//  Mutex.c
//  Locks
//
//  Created by 范云飞 on 2024/9/12.
//

#include "Mutex.h"
//// 互斥结构
//typedef struct {
//    atomic_int lock;
//} Mutex;

// 初始化互斥锁
void mutex_init(Mutex *m) {
    atomic_store(&m->lock, 0);  // 0 表示未锁定
}

// 获取互斥锁
void mutex_lock(Mutex *m) {
    while (true) {
        // 内存屏障，确保获取锁操作的顺序性
        atomic_thread_fence(memory_order_acquire);

        int expected = 0;
        // 使用 Compare-and-Swap 尝试获取锁
        if (atomic_compare_exchange_strong(&m->lock, &expected, 1)) {
            return;  // 成功获取锁，退出循环
        }

        // 如果获取失败，自旋等待
        while (atomic_load(&m->lock) != 1) {
            // 可以插入空操作以减少忙等待导致的性能损耗
        }
    }
}

// 释放互斥锁
void mutex_unlock(Mutex *m) {
    // 内存屏障，确保释放锁操作的顺序性
    atomic_thread_fence(memory_order_release);

    // 释放锁，将锁状态重置为 0
    atomic_store(&m->lock, 0);
}
