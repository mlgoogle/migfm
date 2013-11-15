#ifndef __BASE_THREADPOOL_H__
#define __BASE_THREADPOOL_H__

#include "base_config.h"
#include "base_thread.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    typedef struct base_threadpool_t	base_threadpool_t;

    typedef void *(*base_threadpool_run_t)(base_thread_t*, int, void*);

// 初始化
    int base_init_threadpool(base_threadpool_t ** me,
                             size_t init_threads,
                             size_t max_threads,
                             size_t max_tasks);

// 销毁
    int base_deinit_threadpool(base_threadpool_t *me);

// 添加一个优先任务
    int base_threadpool_addtask(base_threadpool_t *me,
                                base_threadpool_run_t func,
                                int flag,
                                void *param,
                                int priority,
                                void *owner);

// 添加一个定时任务
    int base_threadpool_scheduletask(base_threadpool_t *me,
                                     base_threadpool_run_t func,
                                     int flag,
                                     void *param,
                                     microsecond_t time,
                                     void *owner);

// 提升任务为本级别最优先
    int base_threadpool_top(base_threadpool_t *me,
                            base_threadpool_run_t func,
                            int flag,
                            void *param,
                            int priority,
                            void *owner);

// 取消任务
    int base_threadpool_cancel_tasks(base_threadpool_t *me,
                                     void *owner);

// 剩余优先级任务数
    size_t base_threadpool_tasks_count(base_threadpool_t *me);

// 剩余定时任务数
    size_t base_threadpool_scheduled_tasks_count(base_threadpool_t *me);

// 线程数
    size_t base_threadpool_threads_count(base_threadpool_t *me);

// 工作线程数
    size_t base_threadpool_busy_count(base_threadpool_t *me);

// 空闲线程数
    size_t base_threadpool_idle_count(base_threadpool_t *me);

// 己运行的任务数
    size_t base_threadpool_tasks_run_count(base_threadpool_t * me);

// 任务数峰值
    size_t base_threadpool_tasks_high_count(base_threadpool_t * me);

// 线程数峰值
    size_t base_threadpool_threads_high_count(base_threadpool_t * me);

// 空闲线程数
    size_t base_threadpool_get_idle_max(base_threadpool_t *me);

// 设置空闲线程数
    size_t base_threadpool_set_idle_max(base_threadpool_t *me,
                                        size_t cnt);

// 取最大线程数
    size_t base_threadpool_get_thread_max(base_threadpool_t *me);

// 设置最大线程数
    size_t base_threadpool_set_thread_max(base_threadpool_t *me,
                                          size_t cnt);

#ifdef __cplusplus
}
#endif

#endif /* __BASE_THREADPOOL_H__ */
