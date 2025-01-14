#ifndef USER_CONTROLLER_H
#define USER_CONTROLLER_H

#include "../models/UserModel.h"
#include "../views/JsonView.h"
#include <httplib.h>  // 使用 cpp-httplib 库
#include <vector>

class UserController {
public:
    UserController();

    // 路由处理函数
    void getUser(const httplib::Request &req, httplib::Response &res);
    void createUser(const httplib::Request &req, httplib::Response &res);
    void listUsers(const httplib::Request &req, httplib::Response &res); // 新增列表用户的路由
    void addUser(const httplib::Request &req, httplib::Response &res);  // 新增添加用户的路由
};

#endif // USER_CONTROLLER_H