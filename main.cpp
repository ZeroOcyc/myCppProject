#include "ThreadPool.h"
/*测试*/
int main(int argc, char const *argv[])
{
    pool_init(3); /*创建n个线程*/

    /*添加n个任务*/
    int* jobNum = (int*)malloc(sizeof(int) * 10);       /* 一定要动态创建 */
    int i;
    for (i = 0; i < 10; ++i)
    {
        jobNum[i] = i;
        pool_add_job(myprocess, &jobNum[i]);
    }

    sleep(5); /*等待所有任务完成*/

    pool_destroy(); /*销毁线程池*/
    free(jobNum);
    jobNum = NULL;
    return 0;
}