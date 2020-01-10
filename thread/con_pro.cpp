/*=============================================================== 
*   Copyright (C) . All rights reserved.")
*   文件名称： 
*   创 建 者：zhang
*   创建日期：
*   描    述：使用信号量实现生产者与消费者模型 
================================================================*/

#include <cstdio>
#include <cstdlib>
#include <vector>
#include <pthread.h>
#include <semaphore.h>

#define MAX_QUEUE 5
class RingQueue
{
    public:
        RingQueue(int maxq = MAX_QUEUE):
            _capacity(maxq), _array(maxq), 
            _pos_read(0), _pos_write(0){
            sem_init(&_sem_data, 0, 0);//数据资源计数器
            sem_init(&_sem_space, 0, maxq);//空闲空间计数器
            sem_init(&_sem_lock, 0, 1);//锁的初始化
        }
        ~RingQueue(){
            sem_destroy(&_sem_data);
            sem_destroy(&_sem_space);
            sem_destroy(&_sem_lock);
        }
        bool Push(int &data){
            //没有空间空间则直接阻塞,并且空闲空间计数-1
            sem_wait(&_sem_space);//直接通过自身计数判断是否阻塞

            sem_wait(&_sem_lock);//加锁，保护入队操作
            _array[_pos_write] = data;
            _pos_write = (_pos_write + 1) % _capacity;
            sem_post(&_sem_lock);//解锁

            sem_post(&_sem_data);//数据资源计数+1，唤醒消费者 

            return true;
        }
        bool Pop(int *data) {
            sem_wait(&_sem_data);//通过资源计数判断是否能获取资源

            sem_wait(&_sem_lock);
            *data = _array[_pos_read];
            _pos_read = (_pos_read + 1) % _capacity;
            sem_post(&_sem_lock);

            sem_post(&_sem_space);
            return true;
        }
    private:
        std::vector<int> _array;
        int _capacity;
        int _pos_write;
        int _pos_read;
        sem_t _sem_space;
        sem_t _sem_data;
        sem_t _sem_lock;
};
#define MAX_THR 4

void *productor(void *arg)
{
    RingQueue *q = (RingQueue *)arg;
    int i = 0; 
    while(1) {
        //生产者入队数据
        q->Push(i);
        printf("productor %p --- put data:%d\n", 
                pthread_self(), i++);
    }
    return NULL;
}
void *consumer(void *arg)
{
    RingQueue *q = (RingQueue *)arg;
    while(1) {
        //消费者出队数据
        int data;
        q->Pop(&data);
        printf("consumer %p --- get data:%d\n",
                pthread_self(), data);
    }
    return NULL;
}
int main()
{
    int ret;
    pthread_t ptid[MAX_THR], ctid[MAX_THR];
    RingQueue q;

    for (int i = 0; i < MAX_THR; i++) {
        ret = pthread_create(&ptid[i], NULL, productor, (void*)&q);
        if (ret != 0) {
            printf("thread create error\n");
            return -1;
        }
    }
    for (int i = 0; i < MAX_THR; i++) {
        ret = pthread_create(&ctid[i], NULL, consumer, (void*)&q);
        if (ret != 0) {
            printf("thread create error\n");
            return -1;
        }
    }
    for (int i = 0; i < MAX_THR; i++) {
        pthread_join(ptid[i], NULL);
        pthread_join(ctid[i], NULL);
    }
    return 0;
}
