//
//  main.c
//  Locks
//
//  Created by 范云飞 on 2024/9/12.
//

#include <stdatomic.h>
//#include "Spinlock.h"
//
//#pragma mark  Spinlock
//// 共享资源
//int shared_counter = 0;
//Spinlock lock;
//
//// 线程函数
//void* thread_func(void* arg) {
//    for (int i = 0; i < 100000; i++) {
//        spinlock_lock(&lock);
//        shared_counter++;
//        spinlock_unlock(&lock);
//    }
//    return NULL;
//}
//
//void testSpinlock(void) {
//    pthread_t threads[4];
//    spinlock_init(&lock);
//
//    // 创建线程
//    for (int i = 0; i < 4; i++) {
//        pthread_create(&threads[i], NULL, thread_func, NULL);
//    }
//
//    // 等待线程完成
//    for (int i = 0; i < 4; i++) {
//        pthread_join(threads[i], NULL);
//    }
//
//    printf("Final counter value: %d\n", shared_counter);
//}
//
//int main(void) {
//    /// 自选锁
//    testSpinlock();
//    
//    return 0;
//}

//#include <stdio.h>
//#include <pthread.h>
//
//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
//int condition_met = 0;
//
//void *thread_func(void *arg) {
//    pthread_mutex_lock(&mutex);
//    while (!condition_met) {
//        // 等待条件变量，释放互斥锁
//        pthread_cond_wait(&cond, &mutex);
//    }
//    printf("Condition met, thread proceeding...\n");
//    pthread_mutex_unlock(&mutex);
//    return NULL;
//}
//
//void *signal_thread_func(void *arg) {
//    pthread_mutex_lock(&mutex);
//    // 设置条件满足标志
//    condition_met = 1;
//    // 发出信号，唤醒一个等待的线程
//    pthread_cond_signal(&cond);
//    pthread_mutex_unlock(&mutex);
//    return NULL;
//}
//
//int main(void) {
//    pthread_t thread1, thread2,thread3,thread4,thread5,thread6;
//
//    pthread_create(&thread1, NULL, thread_func, NULL);
//    pthread_create(&thread2, NULL, thread_func, NULL);
//    pthread_create(&thread3, NULL, thread_func, NULL);
//    pthread_create(&thread4, NULL, signal_thread_func, NULL);
//    pthread_create(&thread5, NULL, signal_thread_func, NULL);
//    pthread_create(&thread6, NULL, signal_thread_func, NULL);
//
//    pthread_join(thread1, NULL);
//    pthread_join(thread2, NULL);
//
//    pthread_mutex_destroy(&mutex);
//    pthread_cond_destroy(&cond);
//
//    return 0;
//}


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// 任务结构体
typedef struct Task {
    void (*function)(void *);
    void *arg;
    struct Task *next;
} Task;

// 队列结构体
typedef struct {
    Task *head;
    Task *tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int shutdown;
} Queue;

// 初始化队列
void queue_init(Queue *q) {
    q->head = NULL;
    q->tail = NULL;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
    q->shutdown = 0;
}

// 销毁队列
void queue_destroy(Queue *q) {
    pthread_mutex_lock(&q->mutex);
    Task *task = q->head;
    while (task) {
        Task *next = task->next;
        free(task);
        task = next;
    }
    pthread_mutex_unlock(&q->mutex);
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);
}

// 添加任务到队列
void queue_push(Queue *q, void (*function)(void *), void *arg) {
    Task *newTask = malloc(sizeof(Task));
    newTask->function = function;
    newTask->arg = arg;
    newTask->next = NULL;

    pthread_mutex_lock(&q->mutex);
    if (q->tail) {
        q->tail->next = newTask;
    } else {
        q->head = newTask;
    }
    q->tail = newTask;
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

// 从队列获取任务
Task *queue_pop(Queue *q) {
    pthread_mutex_lock(&q->mutex);
    while (q->head == NULL && !q->shutdown) {
        pthread_cond_wait(&q->cond, &q->mutex);
    }
    if (q->shutdown) {
        pthread_mutex_unlock(&q->mutex);
        return NULL;
    }
    Task *task = q->head;
    q->head = task->next;
    if (q->head == NULL) {
        q->tail = NULL;
    }
    pthread_mutex_unlock(&q->mutex);
    return task;
}

// 处理线程函数
void *worker(void *arg) {
    Queue *q = (Queue *)arg;
    while (1) {
        Task *task = queue_pop(q);
        if (task == NULL) {
            break; // 队列关闭
        }
        task->function(task->arg);
        free(task);
    }
    return NULL;
}

// 启动线程池
void start_workers(Queue *q, pthread_t *workers, int num_workers) {
    for (int i = 0; i < num_workers; i++) {
        pthread_create(&workers[i], NULL, worker, q);
    }
}

// 关闭线程池
void shutdown_workers(Queue *q, pthread_t *workers, int num_workers) {
    pthread_mutex_lock(&q->mutex);
    q->shutdown = 1;
    pthread_cond_broadcast(&q->cond);
    pthread_mutex_unlock(&q->mutex);

    for (int i = 0; i < num_workers; i++) {
        pthread_join(workers[i], NULL);
    }
}

void task_function(void *arg) {
    printf("Task executed with arg: %d\n", *(int *)arg);
}

int main() {
    Queue queue;
    pthread_t workers[4];
    int task_args[4] = {1, 2, 3, 4};

    queue_init(&queue);
    start_workers(&queue, workers, 4);

    for (int i = 0; i < 4; i++) {
        queue_push(&queue, task_function, &task_args[i]);
    }

//    shutdown_workers(&queue, workers, 4);
//    queue_destroy(&queue);

    return 0;
}
