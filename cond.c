//这个demo体会条件变量的基本使用
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

int have_delicacy = 0 ;//0表示没有美食
pthread_cond_t cond;
pthread_mutex_t mutex;
void *gourmet(void * arg)
{
    while(1)
    {
       
         pthread_mutex_lock(&mutex);

        while(have_delicacy ==0)
        {
            //改进后，被唤醒之后应该重新判断有没有面
            //没有美食要等待,但是需要在等待前先解锁
           // pthread_mutex_unlock(&mutex);
            //--------------------------------,存在竟态执行
            //并且这个解锁操作和陷入休眠的操作必须是原子操作
            //解锁-》休眠-》被唤醒之后重新加锁
            pthread_cond_wait(&cond,&mutex);
            //在上面这个函数里面包含了解锁，休眠两步
            //被唤醒之后可以执行下面的代码，如果没有被唤醒那么就会阻塞下面的程序的执行
        }
    /*
        if(have_delicacy ==0)
        {
            //没有美食要等待,但是需要在等待前先解锁
           // pthread_mutex_unlock(&mutex);
            //--------------------------------,存在竟态执行
            //并且这个解锁操作和陷入休眠的操作必须是原子操作
            //解锁-》休眠-》被唤醒之后重新加锁
            pthread_cond_wait(&cond,&mutex);
            //在上面这个函数里面包含了解锁，休眠两步
            //被唤醒之后可以执行下面的代码，如果没有被唤醒那么就会阻塞下面的程序的执行
        }*/
        printf("真香\n");

        have_delicacy--;
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond);
    }
    return NULL;
}

void *chief(void * arg)
{

    while(1)
    {
        pthread_mutex_lock(&mutex);
        while(have_delicacy==1)
        {
            pthread_cond_wait(&cond,&mutex);

        }
        printf("我做了一碗面\n");
      
        have_delicacy++;
        //做出美和之后，唤醒等待的人
        pthread_mutex_unlock(&mutex);//锁是永远保护临界资源的，加锁的部分别的执行流无法访问，保证同一时间内
        //只有一个执行流可以访问操作临界资源
        pthread_cond_signal(&cond);//唤醒等待的执行流
    }
    return NULL;
}

int main()
{
    //初始化条件变量
    pthread_cond_init(&cond,NULL);

    pthread_mutex_init(&mutex,NULL);



    
    pthread_t gourmet_tid ,chief_tid;
    int ret;
    int i=0;
    for( i=0;i<4;++i)
    {
    ret=pthread_create(&gourmet_tid, NULL,gourmet,NULL);
    if(ret !=0)
    {
        printf("pthread create erron\n");
        return -1;

         }
    }
    for( i=0;i<4;++i)
    {
    ret=pthread_create(&chief_tid, NULL,chief,NULL);
    if(ret !=0)
    {
        printf("pthread create erron\n");
        return -1;
    }
    }
    pthread_join(gourmet_tid ,NULL);
    pthread_join(chief_tid,NULL);
    pthread_cond_destroy(&cond);//销毁条件变量
    pthread_mutex_destroy(&mutex);//销毁
    return 0;
}
