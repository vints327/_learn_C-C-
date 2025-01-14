#include "UserModel.h"
#include <iostream> // 模拟输出日志

UserModel::UserModel() : id(0), name("") {}

UserModel::~UserModel() {}

void UserModel::setName(const std::string& name) {
    this->name = name;
}

std::string UserModel::getName() const {
    return this->name;
}

// 模拟数据库操作
bool UserModel::saveToDatabase() const {
    std::cout << "Saving user: " << name << " to database..." << std::endl;
    return true; // 模拟保存成功
}

UserModel UserModel::getUserById(int userId) {
    // 模拟从数据库中获取用户数据
    UserModel user;
    user.id = userId;
    user.name = "John Doe";  // 示例数据
    return user;
}