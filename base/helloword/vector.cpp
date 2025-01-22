#include <iostream>
#include <vector>

int main() {
    std::vector<int> vec = {1, 2, 3, 4, 5};

    vec.push_back(6);  // 添加元素
    vec.pop_back();    // 删除最后一个元素

    for (const auto& elem : vec) {
        std::cout << elem << " ";
    }

    return 0;
}