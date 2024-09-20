//
//  Semaphore.c
//  Locks
//
//  Created by 范云飞 on 2024/9/12.
//

#include "Semaphore.h"

//// 信号量结构
//typedef struct {
//    atomic_int count;
//} Semaphore;

//// 初始化信号量
//void semaphore_init_f(Semaphore *s, int value) {
//    atomic_store(&s->count, value);  // 初始化信号量的计数器
//}
//
//// 获取信号量
//void semaphore_wait_f(Semaphore *s) {
//    while (true) {
//        // 内存屏障，防止编译器对内存操作的重排
//        atomic_thread_fence(memory_order_acquire);
//        
//        // FAA 操作：原子性地减少计数器，并获取之前的值
//        int old_count = atomic_fetch_add(&s->count, -1);
//        if (old_count > 0) {
//            // 成功获取信号量，退出循环
//            break;
//        }
//        
//        // 如果计数器小于等于 0，自旋等待信号量释放
//        atomic_fetch_add(&s->count, 1);  // 恢复计数器
//    }
//}
//
//// 释放信号量
//void semaphore_signal_f(Semaphore *s) {
//    // 内存屏障，确保所有操作完成后再增加计数器
//    atomic_thread_fence(memory_order_release);
//    
//    // FAA 操作：原子性地增加计数器
//    atomic_fetch_add(&s->count, 1);
//}


// 初始化信号量
void semaphore_init_f(Semaphore *s, int value) {
    atomic_store(&s->count, value);  // 设置初始计数器值
}

// P 操作（等待/申请）
void semaphore_wait_f(Semaphore *s) {
    while (true) {
        // 使用内存屏障，确保 P 操作的顺序性
        atomic_thread_fence(memory_order_acquire);

        int current = atomic_load(&s->count);
        if (current > 0) {
            // CAS 尝试原子性地减少计数器
            int expected = current;
            if (atomic_compare_exchange_strong(&s->count, &expected, current - 1)) {
                return;  // 成功获取信号量
            }
        }

        // 如果计数器小于等于零，自旋等待信号量可用
        while (atomic_load(&s->count) <= 0) {
            // 可以插入空操作以减少忙等待导致的性能损耗
            __asm__ __volatile__ ("nop");
        }
    }
}

// V 操作（释放/释放）
void semaphore_signal_f(Semaphore *s) {
    // 使用内存屏障，确保 V 操作的顺序性
    atomic_thread_fence(memory_order_release);

    // CAS 尝试原子性地增加计数器
    int current = atomic_load(&s->count);
    while (!atomic_compare_exchange_strong(&s->count, &current, current + 1)) {
        // 如果 CAS 失败，重新加载计数器的值并重试
    }
}
