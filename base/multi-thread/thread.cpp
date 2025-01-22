#include <iostream>
#include <thread>

void print_message(const std::string& message) {
    std::cout << message <<std::endl;
}

int main() {
    std::thread t1(print_message, "Hello from Thread 1");
    std::thread t2(print_message, "Hello from Thread 2");

    t1.join();  // 等待 t1 完成
    t2.join();  // 等待 t2 完成

    return 0;
}