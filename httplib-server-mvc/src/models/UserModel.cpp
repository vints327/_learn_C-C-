#include "UserModel.h"

User UserModel::getUserById(int id) {
    // 模拟从数据库获取用户信息
    if (id == 1) {
        return {1, "John Doe", "john.doe@example.com"};
    } else {
        return {-1, "", ""};  // 用户不存在
    }
}

bool UserModel::createUser(const User &user) {
    // 模拟创建用户
    return true;  // 假设创建成功
}

std::vector<User> UserModel::getAllUsers() {
    // 模拟获取所有用户
    return {
        {1, "John Doe", "john.doe@example.com"},
        {2, "Jane Smith", "jane.smith@example.com"}
    };
}