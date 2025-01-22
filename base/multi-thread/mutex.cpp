#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

// 定义一个全局互斥锁，用于保护共享资源的访问
std::mutex mtx;
// 定义一个全局变量，作为共享资源
int counter = 0;

/**
 * @brief 增加函数，用于安全地增加全局变量 counter 的值。
 * 
 * 该函数展示了如何在多线程中使用互斥锁来避免竞争条件。
 * 通过使用 lock_guard 自动管理互斥锁的加锁和解锁，
 * 确保每个线程可以安全地增加计数器。
 * 每次循环迭代时，线程会暂停 500 毫秒。
 * 在 ++counter; 语句前后打印出计数器的值、当前时间和当前线程的 ID。
 */
void increment() {
    for (int i = 0; i < 1000; ++i) {
        // 使用 lock_guard 自动加锁和解锁，确保在临界区内安全地增加计数器
        std::lock_guard<std::mutex> lock(mtx);  
        
        // 获取当前时间
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        // 获取当前线程的 ID
        std::thread::id this_id = std::this_thread::get_id();

        // 打印 ++counter; 之前的计数器值、当前时间和当前线程的 ID
        std::cout << "Before increment: Counter = " << counter 
                  << ", Time = " << std::ctime(&now_time_t) 
                  << " + " << now_ms.count() << " ms" 
                  << ", Thread ID = " << this_id 
                  << std::flush<<std::endl;

        ++counter;

        // 获取当前时间
        now = std::chrono::system_clock::now();
        now_time_t = std::chrono::system_clock::to_time_t(now);
        now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        // 打印 ++counter; 之后的计数器值、当前时间和当前线程的 ID
        std::cout << "After increment: Counter = " << counter 
                  << ", Time = " << std::ctime(&now_time_t) 
                  << " + " << now_ms.count() << " ms" 
                  << ", Thread ID = " << this_id 
                  << std::flush<<std::endl;

        // 延时 500 毫秒
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

/**
 * @brief 主函数，演示如何使用线程并发地增加共享计数器。
 * 
 * 该函数创建两个线程，分别执行 increment 函数。
 * 在两个线程都完成后，输出计数器的最终值。
 */
int main() {
    // 创建两个线程，分别执行 increment 函数
    std::thread t1(increment);
    std::thread t2(increment);

    // 等待两个线程完成
    t1.join();
    t2.join();

    // 输出计数器的最终值
    std::cout << "Final Counter Value: " << counter << "\n";
    return 0;
}