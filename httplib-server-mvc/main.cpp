#include "./src/controllers/UserController.h"
#include <iostream>
#include "httplib.h"  // 使用 cpp-httplib 库

int main() {
    httplib::Server svr;

    UserController userController;

    // 配置二级路由
    svr.Get("/user/:id", [&](const httplib::Request &req, httplib::Response &res) {
        userController.getUser(req, res);  // 传递 req 和 res 参数
    });

    svr.Post("/user", [&](const httplib::Request &req, httplib::Response &res) {
        userController.createUser(req, res);
    });

    svr.Get("/user/list", [&](const httplib::Request &req, httplib::Response &res) {
        userController.listUsers(req, res);  // 显示用户列表
    });

    svr.Post("/user/add", [&](const httplib::Request &req, httplib::Response &res) {
        userController.addUser(req, res);  // 添加新用户
    });

    std::cout << "Server started on http://localhost:8080" << std::endl;
    svr.listen("localhost", 8080);

    return 0;
}