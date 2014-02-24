#include "base_ring.h"
#include "base_thread.h"
#include "base_threadlock.h"
#include "base_threadpool.h"

microsecond_t base_time_now (void)
{
    struct timeval tv;
    gettimeofday (&tv, NULL);
    return tv.tv_sec * _USEC_PER_SEC + tv.tv_usec;
}

#define TASK_PRIORITY_SEGS      4
#define TASK_PRIORITY_SEG(x)    (((x)->dispatch.priority & 0xFF) / 64)

typedef struct base_thread_pool_task {
    BASE_RING_ENTRY(base_thread_pool_task) link;
    base_threadpool_run_t func;
    int flag;
    void *param;
    void *owner;
    union {
        int priority;
        microsecond_t time;
    } dispatch;
} base_thread_pool_task_t;

BASE_RING_HEAD(base_thread_pool_tasks, base_thread_pool_task);

struct base_thread_list_elt {
    BASE_RING_ENTRY(base_thread_list_elt) link;
    base_thread_t *thd;
    volatile void *current_owner;
    volatile enum {
        TH_RUN/*运行*/, TH_STOP/*退出*/, TH_PROBATION/*临时*/
    } state;
};

BASE_RING_HEAD(base_thread_list, base_thread_list_elt);

struct base_threadpool_t {
    volatile size_t thd_max;
    volatile size_t idle_max;
    volatile microsecond_t idle_wait;
    volatile size_t thd_cnt;
    volatile size_t idle_cnt; // 空闲线程计数
    volatile size_t task_cnt;
    volatile size_t scheduled_task_cnt;
    volatile size_t threshold;
    volatile size_t tasks_run;
    volatile size_t tasks_high;
    volatile size_t thd_high;
    volatile size_t thd_timed_out;
    struct base_thread_pool_tasks *tasks; // 优先级任务
    struct base_thread_pool_tasks *scheduled_tasks; // 定时任务
    struct base_thread_list *busy_thds; // 工作线程列表
    struct base_thread_list *idle_thds; // 空闲线程列表
    base_threadmutex_t *lock;
    base_threadcond_t *cond;
    volatile int terminated;
    struct base_thread_pool_tasks *recycled_tasks;  // 己回收任务对象
    struct base_thread_list *recycled_thds;         // 己回收线程对象
    base_thread_pool_task_t *task_idx[TASK_PRIORITY_SEGS];
    volatile size_t task_max;
};

/* 初始化线程池对象，初始化限制值、计数、任务队列及线程队列等 */
static int thread_pool_construct(base_threadpool_t * me,
                                 size_t init_threads,
                                 size_t max_threads,
                                 size_t max_tasks)
{
    int rv;
    int i;

    me->thd_max = max_threads;
    me->idle_max = init_threads;
    me->threshold = init_threads / 2;
    rv = base_init_threadmutex(&me->lock, PTHREAD_MUTEX_RECURSIVE);
    if (0 != rv) {
        return rv;
    }
    rv = base_init_threadcond(&me->cond);
    if (0 != rv) {
        base_deinit_threadmutex(me->lock);
        return rv;
    }

    me->task_max = max_tasks;
    me->tasks = NULL;
    me->scheduled_tasks = NULL;
    me->recycled_tasks = NULL;
    me->busy_thds = NULL;
    me->idle_thds = NULL;
    me->recycled_thds = NULL;

    me->tasks = malloc(sizeof(*me->tasks));
    if (!me->tasks) {
        goto CATCH_ENOMEM;
    }
    BASE_RING_INIT(me->tasks, base_thread_pool_task, link);
    me->scheduled_tasks = malloc(sizeof(*me->scheduled_tasks));
    if (!me->scheduled_tasks) {
        goto CATCH_ENOMEM;
    }
    BASE_RING_INIT(me->scheduled_tasks, base_thread_pool_task, link);
    me->recycled_tasks = malloc(sizeof(*me->recycled_tasks));
    if (!me->recycled_tasks) {
        goto CATCH_ENOMEM;
    }
    BASE_RING_INIT(me->recycled_tasks, base_thread_pool_task, link);
    me->busy_thds = malloc(sizeof(*me->busy_thds));
    if (!me->busy_thds) {
        goto CATCH_ENOMEM;
    }
    BASE_RING_INIT(me->busy_thds, base_thread_list_elt, link);
    me->idle_thds = malloc(sizeof(*me->idle_thds));
    if (!me->idle_thds) {
        goto CATCH_ENOMEM;
    }
    BASE_RING_INIT(me->idle_thds, base_thread_list_elt, link);
    me->recycled_thds = malloc(sizeof(*me->recycled_thds));
    if (!me->recycled_thds) {
        goto CATCH_ENOMEM;
    }
    BASE_RING_INIT(me->recycled_thds, base_thread_list_elt, link);

    me->thd_cnt = me->idle_cnt = me->task_cnt = me->scheduled_task_cnt = 0;
    me->tasks_run = me->tasks_high = me->thd_high = me->thd_timed_out = 0;
    me->idle_wait = 0;  // 空闲等待
    me->terminated = 0; // 是否退出
    for (i = 0; i < TASK_PRIORITY_SEGS; i++) {
        me->task_idx[i] = NULL; // 优先级任务列表
    }
    goto FINAL_EXIT;
CATCH_ENOMEM:
    free(me->tasks);
    free(me->scheduled_tasks);
    free(me->recycled_tasks);
    free(me->busy_thds);
    free(me->idle_thds);
    free(me->recycled_thds);
    rv = ENOMEM;
    base_deinit_threadmutex(me->lock);
    base_deinit_threadcond(me->cond);
FINAL_EXIT:
    return rv;
}

// 取任务，先从定时列表取，如果没有到普通列表取
static base_thread_pool_task_t *pop_task(base_threadpool_t * me)
{
    base_thread_pool_task_t *task = NULL;
    int seg;

    if (me->scheduled_task_cnt > 0) {
        task = BASE_RING_FIRST(me->scheduled_tasks);
        assert(task != NULL);
        assert(task !=
               BASE_RING_SENTINEL(me->scheduled_tasks, base_thread_pool_task,
                                  link));
        if (task->dispatch.time <= base_time_now()) {
            // 任务未过期
            --me->scheduled_task_cnt;
            BASE_RING_REMOVE(task, link);
            return task;
        }
    }

    if (me->task_cnt == 0) {
        return NULL;
    }

    task = BASE_RING_FIRST(me->tasks);
    assert(task != NULL);
    assert(task != BASE_RING_SENTINEL(me->tasks, base_thread_pool_task, link));
    --me->task_cnt;
    seg = TASK_PRIORITY_SEG(task);
    if (task == me->task_idx[seg]) {
        me->task_idx[seg] = BASE_RING_NEXT(task, link);
        if (me->task_idx[seg] == BASE_RING_SENTINEL(me->tasks,
                base_thread_pool_task, link)
            || TASK_PRIORITY_SEG(me->task_idx[seg]) != seg) {
            me->task_idx[seg] = NULL;
        }
    }
    BASE_RING_REMOVE(task, link);
    return task;
}

static microsecond_t waiting_time(base_threadpool_t * me)
{
    base_thread_pool_task_t *task = NULL;

    task = BASE_RING_FIRST(me->scheduled_tasks);
    assert(task != NULL);
    assert(task !=
           BASE_RING_SENTINEL(me->scheduled_tasks, base_thread_pool_task,
                              link));
    return task->dispatch.time - base_time_now();
}

/* 创建新线程对象
 * 如果有己回收的线程对象则使用它，
 * 否则创建新的线程对象。
 */
static struct base_thread_list_elt *elt_new(base_threadpool_t * me,
        base_thread_t * t) {
    struct base_thread_list_elt *elt;

    if (BASE_RING_EMPTY(me->recycled_thds, base_thread_list_elt, link)) {
        elt = calloc(1, sizeof(*elt));
        if (NULL == elt) {
            return NULL;
        }
    } else {
        elt = BASE_RING_FIRST(me->recycled_thds);
        BASE_RING_REMOVE(elt, link);
    }

    BASE_RING_ELEM_INIT(elt, link);
    elt->thd = t;
    elt->current_owner = NULL;
    elt->state = TH_RUN;
    return elt;
}


// 从任务队列中取任务，如果没有就进入等待
static void * thread_pool_func(base_thread_t * t, void *param)
{
    int rv = 0;
    base_threadpool_t *me = param;
    base_thread_pool_task_t *task = NULL;
    microsecond_t wait;
    struct base_thread_list_elt *elt;

    base_lock_threadmutex(me->lock);
    elt = elt_new(me, t);
    if (!elt) {
        base_unlock_threadmutex(me->lock);
        base_exit_thread(t, ENOMEM);
    }

    while (!me->terminated && elt->state != TH_STOP) {
        // 如果为空闲线程对象，从列表中移除它
        if (BASE_RING_NEXT(elt, link) != elt) {
            --me->idle_cnt;
            BASE_RING_REMOVE(elt, link);
        }

        BASE_RING_INSERT_TAIL(me->busy_thds, elt, base_thread_list_elt, link);
        task = pop_task(me);
        while (NULL != task && !me->terminated) {
            ++me->tasks_run;
            elt->current_owner = task->owner;
            base_unlock_threadmutex(me->lock);
            task->func(t, task->flag, task->param);
            base_lock_threadmutex(me->lock);
            BASE_RING_INSERT_TAIL(me->recycled_tasks, task,
                                  base_thread_pool_task, link);
            elt->current_owner = NULL;
            if (TH_STOP == elt->state) {
                break;
            }
            //fix
            /*if (task){
                free(task);
                task = NULL;
	        }*/

            task = pop_task(me);
        }
        assert(NULL == elt->current_owner);
        if (TH_STOP != elt->state)
            BASE_RING_REMOVE(elt, link);

        // 如果空闲且指定了空闲限制，关闭线程
        if ((me->idle_cnt >= me->idle_max
             && !(me->scheduled_task_cnt && 0 >= me->idle_max)
             && !me->idle_wait)
            || me->terminated || elt->state != TH_RUN) {
            --me->thd_cnt;
            if ((TH_PROBATION == elt->state) && me->idle_wait)
                ++me->thd_timed_out;
            BASE_RING_INSERT_TAIL(me->recycled_thds, elt,
                                  base_thread_list_elt, link);
            base_unlock_threadmutex(me->lock);
            base_detach_thread(t);
            base_exit_thread(t, 0);
            return NULL;
        }

        // 成为空闲线程
        ++me->idle_cnt;
        BASE_RING_INSERT_TAIL(me->idle_thds, elt, base_thread_list_elt, link);

        if (me->scheduled_task_cnt)
            wait = waiting_time(me);
        else if (me->idle_cnt > me->idle_max) {
            wait = me->idle_wait;
            elt->state = TH_PROBATION;
        } else
            wait = -1;

        if (wait >= 0) {
            rv = base_timedwait_threadcond(me->cond, me->lock, wait);
        } else {
            rv = base_wait_threadcond(me->cond, me->lock);
        }
    }

    // 线程退出
    --me->thd_cnt;
    base_unlock_threadmutex(me->lock);
    base_exit_thread(t, 0);
    return NULL;
}

// 通知所有线程退出
static int thread_pool_cleanup(base_threadpool_t *_myself)
{
    _myself->terminated = 1;
    base_threadpool_set_idle_max(_myself, 0);
    while (_myself->thd_cnt) {
        base_sleep(20 * 1000);  // 每次20毫秒，直到所有线程退出
    }
    base_deinit_threadmutex(_myself->lock);
    base_deinit_threadcond(_myself->cond);

    free(_myself->tasks);
    free(_myself->scheduled_tasks);
    free(_myself->recycled_tasks);
    free(_myself->busy_thds);
    free(_myself->idle_thds);
    free(_myself->recycled_thds);
    free(_myself);

    return 0;
}

/* 创建新任务
 * 如果有己回收的任务对象则使用它，
 * 否则创建新的任务对象。
 */
static base_thread_pool_task_t *task_new(base_threadpool_t * me,
        base_thread_run_t func,
        int flag, void *param, int priority,
        void *owner, microsecond_t time)
{
    base_thread_pool_task_t *t;

    if (BASE_RING_EMPTY(me->recycled_tasks, base_thread_pool_task, link)) {
        t = calloc(1, sizeof(*t));
        if (NULL == t) {
            return NULL;
        }
    } else {
        t = BASE_RING_FIRST(me->recycled_tasks);
        BASE_RING_REMOVE(t, link);
    }

    BASE_RING_ELEM_INIT(t, link);
    t->func = func;
    t->flag = flag;
    t->param = param;
    t->owner = owner;
    if (time > 0) {
        t->dispatch.time = base_time_now() + time;
    } else {
        t->dispatch.priority = priority;
    }
    return t;
}

static base_thread_pool_task_t *add_if_empty(base_threadpool_t * me,
        base_thread_pool_task_t * const t)
{
    int seg;
    int next;
    base_thread_pool_task_t *t_next;

    seg = TASK_PRIORITY_SEG(t);
    if (me->task_idx[seg]) {
        assert(BASE_RING_SENTINEL(me->tasks, base_thread_pool_task, link) !=
               me->task_idx[seg]);
        t_next = me->task_idx[seg];
        while (t_next->dispatch.priority > t->dispatch.priority) {
            t_next = BASE_RING_NEXT(t_next, link);
            if (BASE_RING_SENTINEL(me->tasks, base_thread_pool_task, link) ==
                t_next) {
                return t_next;
            }
        }
        return t_next;
    }

    for (next = seg - 1; next >= 0; next--) {
        if (me->task_idx[next]) {
            BASE_RING_INSERT_BEFORE(me->task_idx[next], t, link);
            break;
        }
    }
    if (0 > next) {
        BASE_RING_INSERT_TAIL(me->tasks, t, base_thread_pool_task, link);
    }
    me->task_idx[seg] = t;
    return NULL;
}

static int schedule_task(base_threadpool_t *me,
                         base_thread_run_t func, int flag, void *param,
                         void *owner, microsecond_t time)
{
    base_thread_pool_task_t *t;
    base_thread_pool_task_t *t_loc;
    base_thread_t *thd;
    int rv = 0;
    base_lock_threadmutex(me->lock);

    t = task_new(me, func, flag, param, 0, owner, time);
    if (NULL == t) {
        base_unlock_threadmutex(me->lock);
        return ENOMEM;
    }
    t_loc = BASE_RING_FIRST(me->scheduled_tasks);
    while (NULL != t_loc) {
        // 定时任务以时间顺序排列
        if (t->dispatch.time < t_loc->dispatch.time) {
            ++me->scheduled_task_cnt;
            BASE_RING_INSERT_BEFORE(t_loc, t, link);
            break;
        } else {
            t_loc = BASE_RING_NEXT(t_loc, link);
            if (t_loc ==
                BASE_RING_SENTINEL(me->scheduled_tasks, base_thread_pool_task,
                                   link)) {
                ++me->scheduled_task_cnt;
                BASE_RING_INSERT_TAIL(me->scheduled_tasks, t,
                                      base_thread_pool_task, link);
                break;
            }
        }
    }

    if (0 == me->thd_cnt) {
        rv = base_init_thread(&thd, NULL, thread_pool_func, me);
        if (0 == rv) {
            ++me->thd_cnt;
            if (me->thd_cnt > me->thd_high)
                me->thd_high = me->thd_cnt;
        }
    }
    // 唤醒等待线程
    base_signal_threadcond(me->cond);
    base_unlock_threadmutex(me->lock);
    return rv;
}

static int add_task(base_threadpool_t *me, base_thread_run_t func,
                    int flag, void *param, int priority, int push,
                    void *owner)
{
    base_thread_pool_task_t *t;
    base_thread_pool_task_t *t_loc;
    base_thread_t *thd;
    int rv = 0;

    base_lock_threadmutex(me->lock);

    if (me->task_max && me->task_cnt >= me->task_max) {
        base_unlock_threadmutex(me->lock);
        return ERANGE;
    }

    t = task_new(me, func, flag, param, priority, owner, 0);
    if (NULL == t) {
        base_unlock_threadmutex(me->lock);
        return ENOMEM;
    }

    t_loc = add_if_empty(me, t);
    if (NULL == t_loc) {
        goto FINAL_EXIT;
    }

    if (push) {
        while (BASE_RING_SENTINEL(me->tasks, base_thread_pool_task, link) !=
               t_loc && t_loc->dispatch.priority >= t->dispatch.priority) {
            t_loc = BASE_RING_NEXT(t_loc, link);
        }
    }
    BASE_RING_INSERT_BEFORE(t_loc, t, link);
    if (!push) {
        if (t_loc == me->task_idx[TASK_PRIORITY_SEG(t)]) {
            me->task_idx[TASK_PRIORITY_SEG(t)] = t;
        }
    }

FINAL_EXIT:
    me->task_cnt++;
    if (me->task_cnt > me->tasks_high)
        me->tasks_high = me->task_cnt;
    if (0 == me->thd_cnt || (0 == me->idle_cnt && me->thd_cnt < me->thd_max &&
                             me->task_cnt > me->threshold)) {
        rv = base_init_thread(&thd, NULL, thread_pool_func, me);
        if (0 == rv) {
            ++me->thd_cnt;
            if (me->thd_cnt > me->thd_high)
                me->thd_high = me->thd_cnt;
        }
		//fix
		//base_deinit_thread(thd);
    }

    // 唤醒等待线程
    base_signal_threadcond(me->cond);
    base_unlock_threadmutex(me->lock);

    return rv;
}

static int remove_scheduled_tasks(base_threadpool_t *me,
                                  void *owner)
{
    base_thread_pool_task_t *t_loc;
    base_thread_pool_task_t *next;

    t_loc = BASE_RING_FIRST(me->scheduled_tasks);
    while (t_loc !=
           BASE_RING_SENTINEL(me->scheduled_tasks, base_thread_pool_task,
                              link)) {
        next = BASE_RING_NEXT(t_loc, link);
        if (t_loc->owner == owner) {
            --me->scheduled_task_cnt;
            BASE_RING_REMOVE(t_loc, link);
        }
        t_loc = next;
    }
    return 0;
}

static int remove_tasks(base_threadpool_t *me, void *owner)
{
    base_thread_pool_task_t *t_loc;
    base_thread_pool_task_t *next;
    int seg;

    t_loc = BASE_RING_FIRST(me->tasks);
    while (t_loc != BASE_RING_SENTINEL(me->tasks, base_thread_pool_task, link)) {
        next = BASE_RING_NEXT(t_loc, link);
        if (t_loc->owner == owner) {
            --me->task_cnt;
            seg = TASK_PRIORITY_SEG(t_loc);
            if (t_loc == me->task_idx[seg]) {
                me->task_idx[seg] = BASE_RING_NEXT(t_loc, link);
                if (me->task_idx[seg] == BASE_RING_SENTINEL(me->tasks,
                        base_thread_pool_task,
                        link)
                    || TASK_PRIORITY_SEG(me->task_idx[seg]) != seg) {
                    me->task_idx[seg] = NULL;
                }
            }
            BASE_RING_REMOVE(t_loc, link);
        }
        t_loc = next;
    }
    return 0;
}

static void wait_on_busy_threads(base_threadpool_t *me, void *owner)
{
    struct base_thread_list_elt *elt;
    base_lock_threadmutex(me->lock);
    elt = BASE_RING_FIRST(me->busy_thds);
    while (elt != BASE_RING_SENTINEL(me->busy_thds, base_thread_list_elt, link)) {
        if (elt->current_owner != owner) {
            elt = BASE_RING_NEXT(elt, link);
            continue;
        }

        while (elt->current_owner == owner) {
            base_unlock_threadmutex(me->lock);
            base_sleep(200 * 1000);
            base_lock_threadmutex(me->lock);
        }
        elt = BASE_RING_FIRST(me->busy_thds);
    }
    base_unlock_threadmutex(me->lock);
    return;
}

static struct base_thread_list_elt *trim_threads(base_threadpool_t *me,
        size_t *cnt, int idle) {
    struct base_thread_list *thds;
    size_t n, n_dbg, i;
    struct base_thread_list_elt *head, *tail, *elt;

    base_lock_threadmutex(me->lock);
    if (idle) {
        thds = me->idle_thds;
        n = me->idle_cnt;
    } else {
        thds = me->busy_thds;
        n = me->thd_cnt - me->idle_cnt;
    }
    if (n <= *cnt) {
        base_unlock_threadmutex(me->lock);
        *cnt = 0;
        return NULL;
    }
    n -= *cnt;

    head = BASE_RING_FIRST(thds);
    for (i = 0; i < *cnt; i++) {
        head = BASE_RING_NEXT(head, link);
    }
    tail = BASE_RING_LAST(thds);
    if (idle) {
        BASE_RING_UNSPLICE(head, tail, link);
        me->idle_cnt = *cnt;
    }

    n_dbg = 0;
    for (elt = head; elt != tail; elt = BASE_RING_NEXT(elt, link)) {
        elt->state = TH_STOP;
        n_dbg++;
    }
    elt->state = TH_STOP;
    n_dbg++;
    assert(n == n_dbg);
    *cnt = n;

    base_unlock_threadmutex(me->lock);

    BASE_RING_PREV(head, link) = NULL;
    BASE_RING_NEXT(tail, link) = NULL;
    return head;
}

// 减少空闲线程数
static size_t trim_idle_threads(base_threadpool_t *me, size_t cnt)
{
    size_t n_dbg;
    struct base_thread_list_elt *elt, *head, *tail;
    int rv;

    elt = trim_threads(me, &cnt, 1);

    base_lock_threadmutex(me->lock);
    base_broadcast_threadcond(me->cond);    // 唤醒等待线程
    base_unlock_threadmutex(me->lock);

    n_dbg = 0;
    if (NULL != (head = elt)) {
        while (elt) {
            tail = elt;
            base_join_thread(&rv, elt->thd); // 回收线程
            elt = BASE_RING_NEXT(elt, link);
            ++n_dbg;
        }
        base_lock_threadmutex(me->lock);
        BASE_RING_SPLICE_TAIL(me->recycled_thds, head, tail,
                              base_thread_list_elt, link);
        base_unlock_threadmutex(me->lock);
    }
    assert(cnt == n_dbg);

    return cnt;
}

// 减少工作线程数
static size_t trim_busy_threads(base_threadpool_t *me, size_t cnt)
{
    trim_threads(me, &cnt, 0);
    return cnt;
}

int base_init_threadpool(base_threadpool_t ** me,
                         size_t init_threads,
                         size_t max_threads,
                         size_t max_tasks)
{
    base_thread_t *t;
    int rv = 0;
    base_threadpool_t *tp;

    *me = NULL;
    tp = calloc(1, sizeof(base_threadpool_t));

    rv = thread_pool_construct(tp, init_threads, max_threads, max_tasks);
    if (0 != rv) {
        free(tp);
        return rv;
    }

    // 创建线程
    while (init_threads) {
        base_lock_threadmutex(tp->lock);
        rv = base_init_thread(&t, NULL, thread_pool_func, tp);
        base_unlock_threadmutex(tp->lock);
        if (0 != rv) {
            break;
        }
        tp->thd_cnt++;
        if (tp->thd_cnt > tp->thd_high) {
            tp->thd_high = tp->thd_cnt;
        }
        --init_threads;
    }

    if (rv == 0) {
        *me = tp;
    }

    return rv;
}

int base_deinit_threadpool(base_threadpool_t *me)
{
    return thread_pool_cleanup (me);
}

// 添加一个优先任务
int base_threadpool_addtask(base_threadpool_t *me,
                            base_threadpool_run_t func,
                            int flag,
                            void *param,
                            int priority,
                            void *owner)
{
    return add_task(me, func, flag, param, priority, 1, owner);
}

// 添加一个定时任务
int base_threadpool_scheduletask(base_threadpool_t *me,
                                 base_threadpool_run_t func,
                                 int flag,
                                 void *param,
                                 microsecond_t time,
                                 void *owner)
{
    return schedule_task(me, func, flag, param, owner, time);
}

// 提升任务为本优先级最先
int base_threadpool_top(base_threadpool_t *me,
                        base_threadpool_run_t func,
                        int flag,
                        void *param,
                        int priority,
                        void *owner)
{
    return add_task(me, func, flag, param, priority, 0, owner);
}

// 取消任务
int base_threadpool_cancel_tasks(base_threadpool_t *me,
                                 void *owner)
{
    int rv = 0;

    base_lock_threadmutex(me->lock);
    if (me->task_cnt > 0) {
        rv = remove_tasks(me, owner);
    }
    if (me->scheduled_task_cnt > 0) {
        rv = remove_scheduled_tasks(me, owner);
    }
    base_unlock_threadmutex(me->lock);
    wait_on_busy_threads(me, owner);

    return rv;
}

// 优先级任务数
size_t base_threadpool_tasks_count(base_threadpool_t *me)
{
    return me->task_cnt;
}

// 定时任务数
size_t base_threadpool_scheduled_tasks_count(base_threadpool_t *me)
{
    return me->scheduled_task_cnt;
}

// 线程数
size_t base_threadpool_threads_count(base_threadpool_t *me)
{
    return me->thd_cnt;
}

// 工作线程数
size_t base_threadpool_busy_count(base_threadpool_t *me)
{
    return me->thd_cnt - me->idle_cnt;
}

// 空闲线程数
size_t base_threadpool_idle_count(base_threadpool_t *me)
{
    return me->idle_cnt;
}

// 己运行的任务数
size_t base_threadpool_tasks_run_count(base_threadpool_t * me)
{
    return me->tasks_run;
}

// 任务数峰值
size_t base_threadpool_tasks_high_count(base_threadpool_t * me)
{
    return me->tasks_high;
}

// 线程数峰值
size_t base_threadpool_threads_high_count(base_threadpool_t * me)
{
    return me->thd_high;
}

size_t base_threadpool_threads_idle_timeout_count(base_threadpool_t * me)
{
    return me->thd_timed_out;
}

// 空闲线程数
size_t base_threadpool_get_idle_max(base_threadpool_t *me)
{
    return me->idle_max;
}

microsecond_t base_threadpool_get_idle_wait(base_threadpool_t * me)
{
    return me->idle_wait;
}

// 设置空闲线程数
size_t base_threadpool_set_idle_max(base_threadpool_t *me,
                                    size_t cnt)
{
    me->idle_max = cnt;
    cnt = trim_idle_threads(me, cnt);
    return cnt;
}

microsecond_t base_threadpool_set_idle_wait(base_threadpool_t * me,
        microsecond_t timeout)
{
    microsecond_t oldtime;

    oldtime = me->idle_wait;
    me->idle_wait = timeout;

    return oldtime;
}

// 取最大线程数
size_t base_threadpool_get_thread_max(base_threadpool_t *me)
{
    return me->thd_max;
}

// 设置最大线程数
size_t base_threadpool_set_thread_max(base_threadpool_t *me,
                                      size_t cnt)
{
    unsigned int n;

    me->thd_max = cnt;
    if (0 == cnt || me->thd_cnt <= cnt) {
        return 0;
    }

    n = me->thd_cnt - cnt;
    if (n >= me->idle_cnt) {
        trim_busy_threads(me, n - me->idle_cnt);
        trim_idle_threads(me, 0);
    } else {
        trim_idle_threads(me, me->idle_cnt - n);
    }
    return n;
}

size_t base_threadpool_get_threshold(base_threadpool_t *me)
{
    return me->threshold;
}

size_t base_threadpool_set_threshold(base_threadpool_t *me,
                                     size_t val)
{
    size_t ov;

    ov = me->threshold;
    me->threshold = val;
    return ov;
}
