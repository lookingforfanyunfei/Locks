//
//  Spinlock.c
//  Locks
//
//  Created by 范云飞 on 2024/9/12.
//

#include "Spinlock.h"

//// 自旋锁结构
//typedef struct {
//    atomic_int lock;
//} Spinlock;

// 初始化自旋锁
void spinlock_init(Spinlock *s) {
    atomic_store(&s->lock, 0);
}

// 获取自旋锁
void spinlock_lock(Spinlock *s) {
    while (true) {
        // 内存屏障，防止获取锁后内存访问顺序被重排
        atomic_thread_fence(memory_order_acquire);
        
        int expected = 0;
        // 尝试将 lock 从 0 更新为 1
        if (atomic_compare_exchange_strong(&s->lock, &expected, 1)) {
            // 成功获取锁
            break;
        }
        // 自旋等待锁释放
        while (atomic_load(&s->lock) != 0) {
            // 插入空操作，减少忙等待
            __asm__ __volatile__ ("nop");
        }
    }
}

// 释放自旋锁
void spinlock_unlock(Spinlock *s) {
    // 内存屏障，确保释放锁之前的所有操作已完成
    atomic_thread_fence(memory_order_release);
    // 释放锁，将锁的状态重置为 0
    atomic_store(&s->lock, 0);
}
