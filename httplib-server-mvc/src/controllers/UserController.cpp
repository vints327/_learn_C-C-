#include "UserController.h"
#include <nlohmann/json.hpp>  // 使用 nlohmann/json 库来处理 JSON

using json = nlohmann::json;

UserController::UserController() {
    // 初始化用户模型或其他资源
}

void UserController::getUser(const httplib::Request &req, httplib::Response &res) {
    // 假设我们有一个方法从 UserModel 获取用户信息
    UserModel userModel;
    
    try {
        int id = std::stoi(req.get_param_value("id"));
        User user = userModel.getUserById(id);

        if (user.id == -1) {
            res.status = 404;
            res.set_content(JsonView::error("User not found"), "application/json");
        } else {
            res.set_content(JsonView::user(user), "application/json");
        }
    } catch (const std::invalid_argument& e) {
        res.status = 400;
        res.set_content(JsonView::error("Invalid ID format"), "application/json");
    } catch (const std::out_of_range& e) {
        res.status = 400;
        res.set_content(JsonView::error("ID out of range"), "application/json");
    }
}

void UserController::createUser(const httplib::Request &req, httplib::Response &res) {
    // 假设我们从请求体中解析用户信息
    json userJson = json::parse(req.body);
    User user;
    user.name = userJson["name"];
    user.email = userJson["email"];

    UserModel userModel;
    bool success = userModel.createUser(user);

    if (success) {
        res.status = 201;
        res.set_content(JsonView::user(user), "application/json");
    } else {
        res.status = 500;
        res.set_content(JsonView::error("Failed to create user"), "application/json");
    }
}

void UserController::listUsers(const httplib::Request &req, httplib::Response &res) {
    UserModel userModel;
    std::vector<User> users = userModel.getAllUsers();

    res.set_content(JsonView::users(users), "application/json");
}

void UserController::addUser(const httplib::Request &req, httplib::Response &res) {
    // 这个方法可以与 createUser 方法合并，或者根据具体需求实现不同的逻辑
    createUser(req, res);
}