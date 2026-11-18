#include <mutex>

class uniquelock{
public:
    uniquelock(Mutex* mutex):mutex_(mutex){
        mutex_->lock();//获取立即加锁
        owns_=true;//持有锁
    }

    uniquelock(const uniquelock& other)=delete;
    uniquelock& operator=(const uniquelock& other)=delete;

    //other.mutex_是一个指针,怎么转移？
    //owns_也可以初始化为true
    uniquelock(uniquelock&& other):mutex_(other.mutex_),owns_(other.owns_){
        other.mutex_=nullptr;
        other.owns_=false;
    }

    //this是个指针那么这里的返回值是？
    uniquelock& operator=(uniquelock&& other){
        if(this==&other)    return *this;

        if(owns_){
            mutex_->unlock();
        }
        
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
    void lock(){
        if(!owns_){
            mutex_->lock();
            owns_=true;
        }
    }


    void unlock() {
        if(owns_){
            mutex_->unlock();
            owns_=false;    
        }   
    }

    //如何把锁转移给别的？

private:
    //这个要怎么写?外来的这个mutex指针
    //(std::mutex)* mutex_;
    Mutex* mutex_;
    bool owns_;//是否持有锁
};