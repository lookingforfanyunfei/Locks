//
//  Semaphore.h
//  Locks
//
//  Created by 范云飞 on 2024/9/12.
//

#ifndef Semaphore_h
#define Semaphore_h

#include <stdatomic.h>
#include <stdbool.h>

// 信号量结构
typedef struct {
    atomic_int count;
} Semaphore;

// 初始化信号量
void semaphore_init_f(Semaphore *s, int value);
// 获取信号量
void semaphore_wait_f(Semaphore *s);

// 释放信号量
void semaphore_signal_f(Semaphore *s);

#endif /* Semaphore_h */
