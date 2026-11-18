核心设计点解析
RAII 与异常安全：
构造时自动调用 lock()，利用 C++ 对象生命周期机制，在离开作用域时自动调用析构函数执行 unlock()。即使在临界区内抛出了异常，栈展开（Stack Unwinding）也能保证析构函数被调用，从而彻底避免死锁。

为什么禁用拷贝和移动？：
如果允许浅拷贝，两个 LockGuard 对象会指向并尝试解锁同一个 mutex_，这会导致未定义行为（通常是重复解锁报错）。因此必须将拷贝构造与拷贝赋值设为 = delete。

模板化设计 (typename Mutex)：
通过模板支持标准库的各种互斥量类型（如 std::mutex、std::recursive_mutex、std::timed_mutex 等），只要它们满足锁的规范（提供 .lock() 和 .unlock() 方法）。