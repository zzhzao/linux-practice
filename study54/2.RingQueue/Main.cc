#include "RingQueue.hpp"

Mutex cnt_lock;
Mutex screen_lock;
int data;
void Print(const std::string &name,const std::string &info)
{
    screen_lock.Lock();
    //print

    screen_lock.Unlock();
}

int Getdata()
{
    cnt_lock.Lock();
    int result = data++;
    cnt_lock.Unlock();
    return result;
}
class ThreadData
{
    public:
    ThreadData(RingQueue<int>* r,const std::string &n):rq(r),name(n)
    {

    }
    ~ThreadData()
    {

    }
public:
    std::string name;
    RingQueue<int> *rq;
};

void *ProductorRoutine(void *args)
{
    RingQueue<int> *rq = static_cast<RingQueue<int> *>(args);
    int data = 1;
    while (true)
    {
        rq->Enqueue(data);
        std::cout << "生产 ：" << data++ << std::endl;
    }
}
void *ConsumerRoutine(void *args)
{
    RingQueue<int> *rq = static_cast<RingQueue<int> *>(args);
    int data = 0;
    while(true)
    {
        rq->Pop(&data);
        std::cout << "消费 ：" << data << std::endl;
    }
}
int main()
{

    RingQueue<int> *rq = new RingQueue<int>();

    pthread_t c[2], p[3];
    pthread_create(p, nullptr, ProductorRoutine, rq);
    pthread_create(p+1, nullptr, ProductorRoutine, rq);
    pthread_create(p+2, nullptr, ProductorRoutine, rq);
    pthread_create(c, nullptr, ConsumerRoutine, rq);
    pthread_create(c+1, nullptr, ConsumerRoutine, rq);

    pthread_join(c[0], nullptr);
    pthread_join(c[1], nullptr);
    pthread_join(p[0], nullptr);
    pthread_join(p[1], nullptr);
    pthread_join(p[2], nullptr);

    return 0;
}