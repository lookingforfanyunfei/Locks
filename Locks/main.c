//
//  main.c
//  Locks
//
//  Created by 范云飞 on 2024/9/12.
//

#include <stdatomic.h>
#include "Spinlock.h"

#pragma mark  Spinlock
// 共享资源
int shared_counter = 0;
Spinlock lock;

// 线程函数
void* thread_func(void* arg) {
    for (int i = 0; i < 100000; i++) {
        spinlock_lock(&lock);
        shared_counter++;
        spinlock_unlock(&lock);
    }
    return NULL;
}

void testSpinlock(void) {
    pthread_t threads[4];
    spinlock_init(&lock);

    // 创建线程
    for (int i = 0; i < 4; i++) {
        pthread_create(&threads[i], NULL, thread_func, NULL);
    }

    // 等待线程完成
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Final counter value: %d\n", shared_counter);
}

int main(void) {
    /// 自选锁
    testSpinlock();
    
    return 0;
}
