相比于只能在构造时加锁、析构时解锁的 std::lock_guard，std::unique_lock 提供了极高的灵活性：

支持延迟加锁（std::defer_lock）

支持已经加锁的接管（std::adopt_lock）

支持手动加锁与解锁（lock() / unlock() / try_lock()）

支持移动语义（Move-only，可以作为函数返回值或存入容器）

支持配合条件变量（std::condition_variable::wait 需要在等待时释放锁，唤醒后重新加锁，只有 unique_lock 能做到）

一、 核心设计与数据成员
一个完整的 SimpleUniqueLock 需要维护两个核心状态：

互斥量指针（Mutex* mtx_）：指向被管理的互斥量。

持有状态标志（bool owns_）：记录当前对象是否真正持有了该锁（因为可能存在延迟加锁、已经释放或通过移动语义交出了所有权的情况）。

三、 关键实现细节与“踩坑”点
移动语义的安全性：
在移动赋值运算符中，必须先检查自己是否持有锁（if (owns_) mtx_->unlock();），防止在覆盖时造成原有的锁泄漏（没有正确解锁）。同时，被移动走的对象（other）必须将指针置空、标志置为 false，防止 other 析构时重复解锁。

状态与异常处理（符合标准库行为）：

如果没有绑定互斥量就调用 lock()，或者在已经持有锁的情况下再次调用 lock()（可能导致死锁），标准库会抛出 std::system_error。

在未持锁时调用 unlock() 也会抛出异常。

为什么需要 release()？：
release() 方法切断了 SimpleUniqueLock 与底层互斥量的联系，并将 owns_ 置为 false，这样在析构时就不会调用 mtx_->unlock()。这允许程序员在跳出作用域后手动管理互斥量的解锁。





    



1. 先明确它解决什么问题
std::unique_lock 是一个 RAII 锁管理器，内部管理一个互斥锁，并记录自己是否拥有该锁。

它和 lock_guard 的主要区别是：unique_lock 可以延迟加锁、手动解锁、重新加锁，还可以转移所有权。
2. 第一版实现目标
先实现一个简化版 MyUniqueLock：
功能	要求
构造函数	接收一个 mutex，立即加锁
析构函数	如果仍然持有锁，就解锁
拷贝构造	禁止
拷贝赋值	禁止
移动构造	转移锁的管理权
移动赋值	先处理自身原有的锁，再接管对方
owns_lock()	判断当前是否持有锁
lock()	加锁
unlock()	解锁

暂时不实现 defer_lock、try_to_lock、adopt_lock 等功能，等基础版本完成后再扩展。
3. 先分析内部数据结构
你应该能想到，核心只需要两个成员：
template <typename Mutex>
class MyUniqueLock {
private:
    Mutex* mutex_;
    bool owns_;
};


这里有两个重要状态：
- mutex_：记录管理的是哪个互斥锁。
- owns_：记录当前是否拥有这个锁。
我们需要始终维护一个核心约束：

owns_ == true 时，mutex_ 必须指向一个有效的互斥锁。