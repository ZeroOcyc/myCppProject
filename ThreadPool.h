#ifndef __THREAD_POOL_
#define __THREAD_POOL_

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
//#include <windows.h>


/* 任务队列 */
//元素：函数，参数指针
typedef struct job
{
    void* (*myprocess) (void* arg); /*任务回调函数*/
    void* arg;                     /*回调函数的参数*/
    struct job* next;              /*任务队列链表*/
	static int job_num;
} CThread_job;

/*回调函数*/
static void* myprocess(void* arg)
{
    printf("threadid: %x, working on task %d\n", pthread_self(), *(int*)arg);      //pthread_self(void)获取线程ID
    sleep(1);
    return(NULL);
}

/*线程池结构*/
typedef struct thread_pool
{
    pthread_mutex_t mutex;      /*互斥锁 */
    pthread_cond_t cond;        /*条件变量 */

    CThread_job* queue_head;    /*线程池的任务队列*/

    int shutdown;               /*是否摧毁线程池 0:不摧毁 1:摧毁 */
    pthread_t* threadid;        /*线程ID数组*/
    int max_thread_num;         /*线程池最大线程数*/
    int cur_queue_size;         /*任务队列中待运行任务数目*/
} CThread_pool;

extern "C"{
/*线程函数*/
void* thread_routine(void* arg);
/*线程池初始化*/
void pool_init(int max_thread_num);
/*将任务加入队列*/
int pool_add_job(void* (*myprocess) (void* arg), void* arg);
/*销毁线程池*/
int pool_destroy();
/*工作线程函数*/
void* thread_routine(void* arg);
/* 往线程池中增加线程 */

int add_thread(CThread_pool *pool, unsigned int add_threads);
/* 从线程池中删除线程 */
int remove_thread(CThread_pool *pool,unsigned int removing_threads);
}
#endif