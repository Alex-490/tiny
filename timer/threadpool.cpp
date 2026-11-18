#include <iostream>
#include <atomic>
#include <chrono>
#include <thread>
#include "threadpool1.h"

int main() {
    std::atomic<int> count{0};

    {
        // 创建 3 个工作线程
        // 最大任务数暂时设为 100
        threadpool pool(3, 100);

        // 提交 10 个任务
        for (int i = 0; i < 10; ++i) {
            pool.enqueue([i, &count]() {

                // 模拟任务耗时
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(100)
                );

                ++count;

                std::cout << "Task " << i
                          << " finished, thread: "
                          << std::this_thread::get_id()
                          << std::endl;
            });
        }

        std::cout << "All tasks submitted!" << std::endl;

        // 不主动等待任务完成
        // 直接离开作用域，触发线程池析构
    }

    std::cout << "Total finished: "
              << count.load() << std::endl;

    return 0;
}