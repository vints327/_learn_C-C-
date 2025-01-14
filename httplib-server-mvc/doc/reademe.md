在 C++ 后端开发中，如果使用 **二级路由**（例如 `/user/add` 和 `/user/list`），可以通过配置控制器和路由来处理不同的请求路径。你可以在控制器中定义多个方法来处理不同的 API 路径，并将它们映射到 HTTP 请求中。

以 **cpp-httplib** 为例，我们可以通过添加更多的路由规则来处理二级路径，如 `/user/add` 和 `/user/list`。下面我将通过扩展前面的 MVC 示例来演示如何实现二级路由。

### **修改后的目录结构**

```
my_cpp_backend_project/
│
├── src/
│   ├── controllers/
│   │   ├── UserController.cpp
│   │   └── UserController.h
│   ├── models/
│   │   ├── UserModel.cpp
│   │   └── UserModel.h
│   ├── views/
│   │   ├── JsonView.cpp
│   │   └── JsonView.h
│   ├── main.cpp
│   └── app.h
├── include/
│   ├── controllers/
│   │   ├── UserController.h
│   ├── models/
│   │   ├── UserModel.h
│   ├── views/
│   │   ├── JsonView.h
└── CMakeLists.txt
```

### **1. 更新控制器（UserController）**

控制器将包含多个方法，每个方法处理不同的路由。在这种情况下，我们要添加 `/user/add` 和 `/user/list` 两个二级路由。

#### `include/controllers/UserController.h`

```cpp
#ifndef USER_CONTROLLER_H
#define USER_CONTROLLER_H

#include "models/UserModel.h"
#include "views/JsonView.h"
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
```

#### `src/controllers/UserController.cpp`

```cpp
#include "controllers/UserController.h"
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
```

### **2. 主程序（Main）**

在主程序中，我们设置二级路由，将 `/user/add` 和 `/user/list` 路径映射到相应的控制器方法。

#### `src/main.cpp`

```cpp
#include <iostream>
#include "controllers/UserController.h"
#include "httplib.h"  // 使用 cpp-httplib 库

int main() {
    httplib::Server svr;

    UserController userController;

    // 配置二级路由
    svr.Get("/user", [&](const httplib::Request &req, httplib::Response &res) {
        userController.getUser(req, res);
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
```

### **3. 完整路由解释**

- **`/user`**（`GET`）: 获取指定 ID 的用户信息。
- **`/user`**（`POST`）: 创建新用户。
- **`/user/list`**（`GET`）: 列出所有用户（模拟数据）。
- **`/user/add`**（`POST`）: 添加新用户。

### **4. 启动和测试**

启动你的服务器后，你可以使用 `curl` 或 Postman 来进行测试：

- 获取用户信息：`GET http://localhost:8080/user?id=1`
- 创建用户：`POST http://localhost:8080/user?name=John`
- 列出所有用户：`GET http://localhost:8080/user/list`
- 添加用户：`POST http://localhost:8080/user/add?name=Jane`

### **总结**

1. 在 C++ 中使用二级路由非常直观，只需要在控制器方法中定义多个方法，并在主程序中通过 `svr.Get` 或 `svr.Post` 映射到相应的 URL 路径即可。
2. `cpp-httplib` 为我们提供了简洁的路由映射方式，可以通过 HTTP 请求的路径来确定调用哪个控制器方法。
3. 使用 MVC 模式来组织代码时，控制器负责管理路由和请求，模型负责数据处理，视图负责展示数据。

通过这种方式，你可以扩展更多的二级路由，并在不同的控制器中处理不同的请求，保持代码的模块化和清晰性。