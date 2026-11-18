#include <mutex>

class uniquelock{
public:
    uniquelock(Mutex* mutex):mutex_(mutex){
        mutex_->lock();//获取立即加锁
        owns_=true;//owns跟锁住与否究竟是什么关系？没锁住难道就不owns呢么
        //应该还是持有锁就是锁住，
    }

    //禁止拷贝构造，为什么要禁止？
    //两个对象持有同一把锁会怎么样?
    //不可能的啊，锁的特性决定的，所以禁止拷贝
    uniquelock(const uniquelock& other)=delete;
    uniquelock& operator=(const uniquelock& other)=delete;//这两个是这样写的么？

    //other.mutex_是一个指针,怎么转移？
    //owns_也可以初始化为true
    uniquelock(uniquelock&& other):mutex_(other.mutex_),owns_(other.owns_){
        other.mutex_=nullptr;
        other.owns_=false;
    }

    //this是个指针那么这里的返回值是？
    uniquelock& operator=(uniquelock&& other){
        if(this==&other)    return *this;

        //这里忘了把之前的资源处理
        if(owns_){
            mutex_->unlock();
        }//unlock()难道是为了这里服务的？移动fuzhi前把原资源处理了？
        
        this->mutex_=other.mutex_;
        this->owns_=other.owns_;
        other.mutex_=nullptr;
        other.owns_=false;
        
        return *this;
    }

    ~uniquelock(){
        if(owns_){
            mutex_->unlock();
            owns_=false;
        }

        //其他资源释放
    }

    //判断还是返回owns_?应该是返回，不然不需要这个成员变量
    bool ownsLock() const{
        return owns_;//那这么写的意义何在呢？让外面的类看？
    }
    
    //既然获取即加锁，那为什么需要这个？
    void lock() const{
        if(owns_)   mutex_.lock();
    }

    void unlock() const{
        if(!owns_) mutex_.unlock();
    }

    //如何把锁转移给别的？

private:
    //这个要怎么写?外来的这个mutex指针
    //(std::mutex)* mutex_;
    Mutex* mutex_;
    bool owns_;//这个拥有是怎么判断？被自己锁着的，还是不是被自己锁着但是
    //说owns_是因为
};