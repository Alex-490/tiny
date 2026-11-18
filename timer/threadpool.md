一、先明确我们要实现什么
线程池的核心思想：
- 创建固定数量的工作线程（Worker）。
- 创建一个任务队列（Task Queue）。
- 外部提交任务，任务进入队列。
- 工作线程不断从队列中取任务并执行。
- 没有任务时，工作线程阻塞等待。
- 线程池析构时，通知所有线程退出，并回收线程资源。

二、实现目标
我们先实现一个基础版本，包含以下功能：
组件	功能
vector<thread>	保存工作线程
queue<function<void()>>	保存待执行任务
mutex	保护任务队列
condition_variable	通知工作线程有新任务
bool stop	控制线程池停止
enqueue()	提交任务

后续再增加 future、packaged_task，支持获取任务返回值。

