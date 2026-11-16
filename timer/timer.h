#include <queue>
#include <functional>
#include <vector>
#include <chrono>
#include <mutex>
#include <cstddef>
#include <map>
#include <unordered_map>
#include <thread>

class Task{
public:
    Task(size_t time):time_(time),alive_(true){

    }
    //允许移动构造么？允许，但好像没必要
    Task();

    //允许拷贝构造么？我猜不允许，因为renwu想想就不能被拷贝

    //把renwu设置为无效
    void setunuseful(){
        alive_=false;
    }

    size_t getTime() const{
        return time_;
    }

    size_t getId() const{
        return id_;
    }
private:
    std::function<void()> fb_; //真正要做的事情，语法不会
    size_t time_; //要执行的时间点
    bool alive_;    //renwu还有效么
    size_t id_;  //renwu 的 id
};

struct cmp{
    bool operator()(Task* a,Task* b) const{
        return a->getTime()>b->getTime();
    }
};

class Timer{
public:
    Timer();
    ~Timer();

    //添加renwu
    void addTimer(Task* task){
        pq_.push(task);
        map_[task->getId()]=task;                             
    }

    size_t minTime () const{
        return pq_.top()->getTime();
    }

    void setUnuse(size_t id){
        Task* tmp=map_[id];
        tmp->setunuseful();
    }

    func(){
        size_t mintime=minTime();
        //这里写的一团糟反正就这个意思。不断检测这个条件成功了就唤醒一个线程
        while(获取当前时间!=mintime){
            wait();
        }
        notify_one();
        //唤醒一个线程怎么执行最顶上的nage renwu
    }

    //pq_确实有个堆顶，但是是怎么检查有没有到时间的？或者说需要有一个函数不断地去比较堆顶数字和现在时间，不可能是堆里每个时间在倒计时，只是保存一个最小时间与当前时间比较
private:
    std::thread worker_;//工作线程，实时监测时间
    std::mutex mutex_;
    std::unordered_map<size_t,Task*> map_;//通过id快速找到task把里面的alive置为false，也许也可以直接做一个mutilset？每次在执行函数里执行前查下有没有这个id，但好像有一个数学关系，好像也没有，直接把task里的东西改了任何时候都不会比这种更复杂
    std::priority_queue<Task*,std::vector<Task*>,cmp> pq_;
    //size_t minTime_;////那我要minTime_做什么呢？构造函数初始化？肯定不是这样的。应该是用上面的minTime获取一个最小时间被某个一直监控时间的函数比较着
};