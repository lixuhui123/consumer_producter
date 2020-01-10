//这个demo体会条件变量的基本使用
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

int have_delicacy = 0 ;//0表示没有美食
pthread_cond_t cond_gourmet;
pthread_cond_t cond_chief;


pthread_mutex_t mutex;
void *gourmet(void * arg)
{
    while(1)
    {
       
         pthread_mutex_lock(&mutex);

        while(have_delicacy ==0)
        {
            pthread_cond_wait(&cond_gourmet,&mutex);
        }
        printf("真香\n");

        have_delicacy--;
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond_chief);
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
            pthread_cond_wait(&cond_chief,&mutex);

        }
        printf("我做了一碗面\n");
      
        have_delicacy++;
        //做出美和之后，唤醒等待的人
        pthread_mutex_unlock(&mutex);//锁是永远保护临界资源的，加锁的部分别的执行流无法访问，保证同一时间内
        //只有一个执行流可以访问操作临界资源
        pthread_cond_signal(&cond_gourmet);//唤醒等待的执行流
    }
    return NULL;
}

int main()
{
    //初始化条件变量
    pthread_cond_init(&cond_chief,NULL);

    pthread_cond_init(&cond_gourmet,NULL);
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
    pthread_cond_destroy(&cond_chief);//销毁条件变量
    pthread_cond_destroy(&cond_gourmet);//销毁条件变量
    pthread_mutex_destroy(&mutex);//销毁
    return 0;
}
