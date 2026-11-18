#include <future>
#include <vector>
#include <queue>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <memory>
#include <shared_mutex>

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
template <typename T,typename... Args>
class threadpool{
public:
    
    threadpool(size_t num,size_t max_tasks):max_threads_(num),max_tasks_(max_tasks),stop_(false){
        for(int i=0;i<max_threads_;i++){
            threads_.emplace_back([this](){
                func();
            });//那为什么还需要这个vector，为了方便回收线程？
        }
    }

    ~threadpool(){
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stop_=true;
        }

        cv_.notify_all();

        for(auto& t:threads_){
            t.join();
        }
    }

    // std::packaged_task：包装任务，并关联返回值。
    // std::future：获取任务执行结果。
    // std::bind：将函数和参数绑定起来。

    //返回值怎么变成万能返回值？
    void enqueue(T func,Args args){
        
        


        {
            std::lock_guard<std::mutex> lock(mutex_);
            if(stop_)   throw std::runtime_error("threadpool stopped");
            tasks_.push();
            
        }
        cv_.notify_one();
    }

    std::packaged_task<int(int, int)> task([](int a, int b) {
        std::this_thread::sleep_for(std::chrono::seconds(1)); // 模拟耗时操作
        return a + b;
    });


    //每个线程要运行的东西
    void func(){
        while (true) {
            // 1. 获取互斥锁
            std::unique_lock<std::mutex> lock(mutex_);//这一步是被notify_one()叫醒的还是while（1）自己醒的
            
            // 2. 等待任务到来，或者线程池停止
            cv_.wait(lock,[&](){
                return stop_||!tasks_.empty(); //其实都卡在这一步等待renwu等待被notify_one()
            });

            // 3. 判断是否应该退出
            if(stop_&&tasks_.empty()) return;   

            // 4. 从任务队列取出一个任务
            std::function<void()> fb=std::move(tasks_.front());
            tasks_.pop();

            // 5. 释放锁
            lock.unlock();

            // 6. 执行任务
            fb();
        }
    }

private:
    std::vector<std::thread> threads_;
    size_t max_threads_;//限制线程数量
    std::queue<std::function<void()>> tasks_;//Task是否应该用某种指针包裹?
    size_t max_tasks_;//限制renwu队列数量
    std::mutex mutex_;
    std::shared_ptr<std::thread> manager_;
    std::condition_variable cv_;
    bool stop_;
};

// workers_ 是一个 vector<thread>，应该用什么方式创建指定数量的线程？

// 每个线程启动时，应该传入什么？这里可以考虑 Lambda。

// 工作线程取任务时，如果队列为空，应该使用什么机制等待，而不是不断循环检查？



//1只要有线程是空闲的就去调renwu做
//2只要有新renwu就去调线程处理

//线程执行完是先放回vector还是直接去看有没有renwu
//需要一个长期存活的线程检查有没有线程是空闲的或者renwu需要处理，有就唤醒一个线程处理队列头renwu

//执行失败的task怎么处理?