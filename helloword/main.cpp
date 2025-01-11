#include <iostream>
#include "greet.h"

int main()
{
    std::cout << "Hello, World!" << std::endl;
    greet("Alice");

    // 等待用户输入，防止窗口一闪而过
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();

    return 0;
}