#include <vector>
#include <queue>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <memory>

//renwu类，对实际要执行的函数进行包装
class task{
public:
    task(std::function<void()> fb):fb_(fb),alive_(true){

    }
    ~task();

private:
    //要执行的函数指针
    std::function<void()> fb_;
    bool alive_;//也许renwu会临时取消
};



class taskQueue{
public:
    taskQueue(size_t max):max_(max){
        
    }

    ~taskQueue(){

    }

    void put(task& t){
        if(queue_.size()==max_){
            //这里怎么处理？
        }
        queue_.push(std::make_shared<task>(std::move(t)));//是这样写么？
    }

private:
    size_t max_;
    std::mutex mutex_;//放renwu是个多线程的行为需要使用mutex？
    std::queue<std::shared_ptr<task>> queue_;//用shared_ptr对么?
};



//renwu流同样是序列容器，这里推荐使用队列，因为防止有的renwu长期被阻塞
//究竟是renwu类调用threadpool对象，还是threadpool调用renwu呢？
//两个流谁主谁次


//threadpool是单例模式
class threadpool{
public:
    threadpool(size_t max_threads,size_t max_tasks):max_threads_(max_threads),max_tasks_(max_tasks){
        //把线程创建好用指针包裹放进threads_
        for(int i=0;i<max_threads_;i++){
            threads_.emplace_back(std::shared_ptr<std::thread>());
        }
        //创建manager线程
    }

    ~threadpool(){

    }

    
    void work(std::shared_ptr<std::thread> t){
        std::shared_ptr<task> ts=tasks_.front();
        tasks_.pop();
        
    }

private:
    //因为是一堆一样的thread所以从序列容器里面选择，vector、queue、deque、链表、string
    //但是为什么选择vector而不是queue呢
    std::vector<std::thread> threads_;//thread这里也是 unique_ptr还是shared_ptr? lockguard能用在这里么？
    size_t max_threads_;//限制线程数量
    std::queue<std::shared_ptr<task>> tasks_;//Task是否应该用某种指针包裹?
    size_t max_tasks_;//限制renwu队列数量
    std::mutex mutex_;
    std::shared_ptr<std::thread> manager_;
    std::condition_variable cv_;

    //后台不断寻找空闲的线程
    void loop(){
        cv_.notify_one();
    }
};

//1只要有线程是空闲的就去调renwu做
//2只要有新renwu就去调线程处理

//线程执行完是先放回vector还是直接去看有没有renwu
//需要一个长期存活的线程检查有没有线程是空闲的或者renwu需要处理，有就唤醒一个线程处理队列头renwu

//执行失败的task怎么处理?