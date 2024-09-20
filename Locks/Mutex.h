//
//  Mutex.h
//  Locks
//
//  Created by 范云飞 on 2024/9/12.
//

#ifndef Mutex_h
#define Mutex_h

#include <stdatomic.h>
#include <stdbool.h>
// 互斥结构
typedef struct {
    atomic_int lock;
} Mutex;

// 初始化互斥锁
void mutex_init(Mutex *m);

// 获取互斥锁
void mutex_lock(Mutex *m) ;
// 释放互斥锁
void mutex_unlock(Mutex *m);


#endif /* Mutex_h */
