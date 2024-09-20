//
//  Spinlock.h
//  Locks
//
//  Created by 范云飞 on 2024/9/12.
//

#ifndef Spinlock_h
#define Spinlock_h

#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>

// 自旋锁结构
typedef struct {
    atomic_int lock;
} Spinlock;

// 初始化自旋锁
void spinlock_init(Spinlock *s);

// 获取自旋锁
void spinlock_lock(Spinlock *s);

// 释放自旋锁
void spinlock_unlock(Spinlock *s);


#endif /* Spinlock_h */
