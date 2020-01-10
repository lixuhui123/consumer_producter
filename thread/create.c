#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

void *thread_start(void *arg)
{
    //pthread_detch(pthread_self());
    char *buf = "leihoua~~\n";
    sleep(3);
    pthread_exit(buf);
    while(1) {
        printf("主线程传递了一个参数:%s\n", (char*)arg);
        sleep(1);
    }
    return NULL;
}

int main()
{
    pthread_t tid;
    char buf[] = "还有半小时~\n";
    int ret = pthread_create(&tid, NULL, thread_start, 
            (void*)buf);
    if  (ret != 0) {
        printf("thread create error:%d\n", ret);
        return -1;
    }
    pthread_detach(tid);
    //sleep(3);
    //pthread_cancel(tid);
    //pthread_exit(NULL);
    //pthread_t pthread_self(void) - 获取当前调用线程id
    //pthread_cancel(pthread_self()); --
    void *retval = NULL;
    pthread_join(tid, &retval);
    printf("retval:%s\n", retval);
    while(1) {
        printf("i am main thread,child tid:%p\n", tid);
        sleep(1);
    }
    return 0;
}
