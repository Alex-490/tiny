#include <mutex>

class lockguard{
public:
    lockguard(){
        mutex_.lock();
    }

    ~lockguard(){
        mutex_.unlock();
    }
private:
    std::mutex mutex_;
};