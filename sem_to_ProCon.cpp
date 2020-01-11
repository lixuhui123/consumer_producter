//使用信号量实现生产者和消费者模型
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<pthread.h>
#include<semaphore.h>

#define QUE_MAX 5 
int j=0;
class RingQueue
{
    public:

        RingQueue(int maxq=QUE_MAX):_capacity(maxq),_array(maxq),_pos_read(0),_pos_write(0)
         {
            sem_init(&_sem_data,0,0);
            sem_init(&_sem_space,0,maxq);
            sem_init(&_sem_lock,0,1);
         }
        ~RingQueue()
        {
            sem_destroy(&_sem_data);
            sem_destroy(&_sem_space);
            sem_destroy(&_sem_lock);

        }
        bool Push(int &data)
        {

            sem_wait(&_sem_space);//没有空闲空间则直接阻塞,并且空闲空间计数-1，直接通过自身判断是否阻塞

            sem_wait(&_sem_lock);//加锁,锁永远只保护临界资源,保护入队操作，信号量自己保护自己的资源

            _array[_pos_write]=data;
            _pos_write = (_pos_write + 1) % _capacity;
            
            sem_post(&_sem_lock);//解锁
            
            sem_post(&_sem_data);//数据资源+1，唤醒消费者
            

            return true;
        }
        bool Pop(int *data)
        {
            //资源计数没有则直接阻塞
            sem_wait(&_sem_data);


            sem_wait(&_sem_lock);

            *data = _array[_pos_read];
            _pos_read=(_pos_read+1) % _capacity;


            sem_post(&_sem_lock);
            sem_post(&_sem_space);
            return true;
        }
    private:

        std::vector<int> _array;
        int _capacity;
        int _pos_write;
        int _pos_read;
        sem_t _sem_space; //生产者
        sem_t _sem_data;  //消费者
        sem_t _sem_lock;
};
#define MAX_THR 4

void *productor(void *arg)
{
    //这是一个线程处理函数，返回值void* 形参任意类型的arg，函数体里面需要类型转换
    RingQueue *q = (RingQueue *)arg;
    while(1)
    {
       usleep(7000);
       //int i=0;
        //生产者入队数据
        q->Push(j);
        printf("productor %p --- put data:%d\n",pthread_self(),++j);



    }
    return NULL;
}

void *consume(void *arg)
{
    //这是一个线程处理函数，返回值void* 形参任意类型的arg，函数体里面需要类型转换
    RingQueue *q = (RingQueue *)arg;
    while(1)
    {
        //消费者出队数据
        int data;
        q->Pop(&data);
        printf("consume %p --- get data:%d\n",pthread_self(),data);
    }
    return NULL;
}




int main()
{
    int ret,i;
    pthread_t ptid[MAX_THR],ctid[MAX_THR];
    RingQueue q;
    for(i=0;i<MAX_THR;++i)
    {
        ret=pthread_create(&ptid[i],NULL,productor,(void*)&q);
        if(ret!=0)
        {
            printf("thread create error\n");
            return -1;
        }
    }

    for(i=0;i<MAX_THR;++i)
    {
        ret=pthread_create(&ctid[i],NULL,consume,(void*)&q);
        if(ret!=0)
        {
            printf("thread create error\n");
            return -1;
        }

    }

    for(i=0;i<MAX_THR;++i)
    {
        pthread_join(ptid[i],NULL);
        pthread_join(ctid[i],NULL);
    }



    return 0;
}
