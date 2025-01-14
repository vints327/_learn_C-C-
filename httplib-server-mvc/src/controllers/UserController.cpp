#include "UserController.h"
#include <httplib.h>
#include <vector>
#include <iostream>

UserController::UserController() {}

void UserController::getUser(const httplib::Request &req, httplib::Response &res) {
    int userId = std::stoi(req.get_param_value("id"));
    UserModel user = UserModel::getUserById(userId);
    std::string jsonResponse = JsonView::renderUser(user.getName());
    res.set_content(jsonResponse, "application/json");
}

void UserController::createUser(const httplib::Request &req, httplib::Response &res) {
    std::string name = req.get_param_value("name");
    UserModel user;
    user.setName(name);
    user.saveToDatabase();
    res.set_content("User created successfully", "text/plain");
}

void UserController::listUsers(const httplib::Request &req, httplib::Response &res) {
    // 模拟用户数据列表
    std::vector<UserModel> users;
    users.push_back(UserModel::getUserById(1)); // 模拟用户数据
    users.push_back(UserModel::getUserById(2));

    // 渲染为 JSON 格式
    nlohmann::json jsonResponse;
    for (const auto& user : users) {
        jsonResponse.push_back({{"id", user.getName()}});
    }

    res.set_content(jsonResponse.dump(), "application/json");
}

void UserController::addUser(const httplib::Request &req, httplib::Response &res) {
    std::string name = req.get_param_value("name");
    UserModel user;
    user.setName(name);
    user.saveToDatabase();
    res.set_content("User added successfully", "text/plain");
}