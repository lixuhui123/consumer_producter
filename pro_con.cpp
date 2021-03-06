#include<iostream>
#include<stdio.h>
#include<unistd.h>
#include<queue>
#include<pthread.h> 

using namespace std;

#define MAX_QUE 5

class BlockQueue
{
private:
    std::queue<int> _queue;
    size_t _capacity;   //限制对队列节点的最大数量
    pthread_mutex_t _mutex;//实现互斥
    pthread_cond_t _cond_pro;  //生产者队列
    pthread_cond_t _cond_con;  //消费者队列
public:
    BlockQueue(int qmax=MAX_QUE):_capacity(qmax)
    {
        // 初始化资源 
        pthread_mutex_init(&_mutex,NULL);
        pthread_cond_init(&_cond_con,NULL);
        pthread_cond_init(&_cond_pro,NULL);
    }
    ~BlockQueue()
    {
        //释放资源
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond_con);
        pthread_cond_destroy(&_cond_pro);
    }
    bool Push(int &data)
    {
        pthread_mutex_lock(&_mutex);
        while(_queue.size()==_capacity)
        {
            pthread_cond_wait(&_cond_pro,&_mutex);
        }
        _queue.push(data);
        pthread_mutex_unlock(&_mutex);
        pthread_cond_signal(&_cond_con);
        return true;

    }
    bool Pop(int * data)
    {
        pthread_mutex_lock(&_mutex);
        while(_queue.empty())
        {
            pthread_cond_wait(&_cond_con,&_mutex);
        }
        *data = _queue.front();
        _queue.pop();
        pthread_mutex_unlock(&_mutex);
        pthread_cond_signal(&_cond_pro);

        return true;
    }

};

void * productor(void *arg)
{
    BlockQueue *q = (BlockQueue *)arg;
    int i=0;
    while(1)
    {
        //生产者不断添加数据
        q->Push(i);
        printf("productor %p --- put data:%d\n",pthread_self(),++i);
    }
    return NULL;
}

void *consume (void *arg)
{
    BlockQueue *q = (BlockQueue*)arg;
    while(1)
    {
        //消费者不断获取数据
        int data;
        q->Pop(&data);
        printf("consume %p --- get data:%d\n",pthread_self(),data);
    }
    return NULL;
}


#define MAX_THR 4
int main()
{
   pthread_t ptid[MAX_THR],ctid[MAX_THR];
   int ret;
   BlockQueue q;
   for(int i=0;i<MAX_THR;++i)
   {
       ret = pthread_create(&ptid[i],NULL,productor,(void*)&q);
       if(ret!=0)
       {
           std::cout << "pthread create error\n";
           return -1;
       }
   }

   for(int i=0;i<MAX_THR;++i)
   {
       ret = pthread_create(&ctid[i],NULL,consume,(void*)&q);
       if(ret!=0)
       {
           std::cout << "pthread create error\n";
           return -1;
       }
   }
    for(int i=0;i<MAX_THR;++i)
    {
        pthread_join(ctid[i],NULL);
        pthread_join(ptid[i],NULL);
    }




    return 0;
}
