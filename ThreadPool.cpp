

// C语言实现方案
// 功能：创建线程池，添加任务，调用空闲线程处理任务，所有任务处理完销毁线程池

/* 基本设定
    任务队列：链表实现
    任务接口：回调函数，任务计数
    线程池管理器：结构体实现
    工作线程：显示当前哪个线程在处理哪个任务
*/

/* 详细代码 */
//主要参考：http://blog.csdn.net/zouxinfox/article/details/3560891


#include "ThreadPool.h"

int job::job_num = 0;
/*线程池实例*/
static CThread_pool* pool = NULL;
int init_max_thread_num = 20;
/*线程池初始化*/
void pool_init(int max_thread_num)
{
    /*一些变量初始化*/
    pool = (CThread_pool*)malloc(sizeof(CThread_pool));

    pthread_mutex_init(&(pool->mutex), NULL);       //互斥锁初始化函数
    pthread_cond_init(&(pool->cond), NULL);         //条件变量初始化函数

    pool->queue_head = NULL;

    pool->max_thread_num = max_thread_num;

    pool->shutdown = 0; /*0打开1关闭*/

    pool->cur_queue_size = 0;

    pool->threadid = (pthread_t*)malloc(init_max_thread_num * sizeof (pthread_t));  //线程ID数组

    /*创建工作线程*/
    int i = 0;
    for (i = 0; i < max_thread_num; ++i)
    {
        pthread_create(&(pool->threadid[i]), NULL, thread_routine, NULL);       //创建相同线程
    }
}

/*将任务加入队列*/
int pool_add_job(void* (*myprocess) (void* arg), void* arg)
{
    /*构造一个新任务*/
    printf("thread pool add task arg: %d\n", *(int*)arg);
    CThread_job* newjob = (CThread_job*)malloc(sizeof(CThread_job));            //新建任务节点
    newjob->myprocess = myprocess;
    newjob->arg = arg;
    newjob->next = NULL;

    /* 线程池的状态被多个线程共享，操作之前需要加锁 */
    pthread_mutex_lock(&(pool->mutex));                                         //上锁，线程池资源绑定执行

    /*将任务加入到任务队列中,也就是链表末端*/
    CThread_job* job = pool->queue_head;
	pool->queue_head->job_num += 1;
    if (job != NULL)
    {
        while (job->next != NULL)
            job = job->next;
        job->next = newjob;                                                     //链表新节点
    }
    else
    {
        pool->queue_head = newjob;
    }

    /*是否需要唤醒线程*/
    int exeSignal = 0;
    if (pool->cur_queue_size == 0)
        exeSignal = 1;

    pool->cur_queue_size += 1;

	if (pool->cur_queue_size > pool->max_thread_num)
	{
		add_thread(pool,1);
	}

    pthread_mutex_unlock(&(pool->mutex));

    /*需要叫醒工作线程*/
    if (exeSignal)
        pthread_cond_signal(&(pool->cond));

    return 0;
}

/*销毁线程池*/
int pool_destroy()
{
    printf("pool destroy now\n");

    /*启用关闭开关*/
    if (pool->shutdown)
        return -1; /*防止两次调用*/
    pool->shutdown = 1;

    /*唤醒所有等待线程*/
    pthread_cond_broadcast(&(pool->cond));

    /*阻塞等待线程退出回收资源，还有另一种办法就是线程分离*/
    int i;
    for (i = 0; i < pool->max_thread_num; ++i)
        pthread_join(pool->threadid[i], NULL);
    free(pool->threadid);
    pool->threadid = NULL;

    /*销毁任务队列*/
    CThread_job* head = NULL;
    while (pool->queue_head != NULL)
    {
        head = pool->queue_head;
        pool->queue_head = pool->queue_head->next;
        free(head);
        head = NULL;
    }

    /*销毁互斥锁与条件变量*/
    pthread_mutex_destroy(&(pool->mutex));
    pthread_cond_destroy(&(pool->cond));

    free(pool);
    pool = NULL;
    printf("pool destroy end\n");
    return 0;
}

/*工作线程函数*/
void* thread_routine(void* arg)
{
    printf("starting threadid: %x\n", pthread_self());

    for (; ;)
    {
        /* 访问线程池之前需要加锁 */
        pthread_mutex_lock(&(pool->mutex));
        /*任务队列为空时wait唤醒,当销毁线程池时跳出循环*/
        while (pool->cur_queue_size == 0 && !pool->shutdown)
        {
            printf("threadid: %x is waiting\n", pthread_self());
            pthread_cond_wait(&(pool->cond), &(pool->mutex));
        }

        /*线程池要销毁了*/
        if (pool->shutdown)             //pthread_cond_broadcast()之后，pool->shutdown置1，销毁线程
        {
            pthread_mutex_unlock(&(pool->mutex));
            printf("threadid: %x will exit\n", pthread_self());
            pthread_exit(NULL);
        }

        /*开始执行任务*/
        printf("threadid: %x is starting to work\n", pthread_self());

        /*等待队列长度减去1，并取出链表中的头元素*/
        pool->cur_queue_size -= 1;
        CThread_job* job = pool->queue_head;
        pool->queue_head = job->next;
        pthread_mutex_unlock(&(pool->mutex));

        /*调用回调函数，执行任务*/
        (*(job->myprocess)) (job->arg);
        free(job);
        job = NULL;
    }
    return(NULL);
}
//主要参考：http://blog.csdn.net/u014453898/article/details/53764720

/* 往线程池中增加线程 */

int add_thread(CThread_pool *pool, unsigned int add_threads)
{
	printf("add_thread.....\n");
    if(add_threads == 0 || pool->max_thread_num >= init_max_thread_num)
        return 0;

    //total_threads: 总线程数，用于循环创建线程时使用
    unsigned int total_threads = pool->max_thread_num + add_threads;

    //actual_add: 为实际上增加的线程数
    int actual_add = 0;

    //创建新线程
    int i;
    for(i = pool->max_thread_num; i < total_threads; i++)
    {
        //创建线程，并判断是否创建成功，失败提示并退出该函数
        if(pthread_create(&(pool->threadid[i]), NULL, thread_routine, NULL) != 0)
        {
            perror("add threads failed!");
            if(actual_add == 0)
                return -1;
        }
        actual_add++;
    }

    //计算增加指定数目的线程后，线程池活动线程数加上实际成功增加的线程数
    pool->max_thread_num += actual_add;
    return actual_add;
}


/* 从线程池中删除线程 */
int remove_thread(CThread_pool *pool,unsigned int removing_threads)
{
    if(removing_threads == 0)
        return 0;

    int remain_threads = pool->max_thread_num - removing_threads;
    remain_threads = remain_threads > 0 ? remain_threads : 1;
    int i;
    for(i = pool->max_thread_num - 1; i > remain_threads - 1; i--)
    {
        int errno = pthread_cancel(pool->threadid[i]);              //删除线程是按照线程创建的顺序来删的
        if(errno != 0)                                          //pthread_cancel成功返回0，失败返回错误编号,分立即取消和在可取消点取消
        {
            break;
        }
    }
    if(i == pool->max_thread_num - 1)
        return -1;
    else
    {
        pool->max_thread_num = i + 1;
        return pool->max_thread_num;
    }
}
