// main.cpp
#include "./src/controllers/UserController.h"
#include <iostream>
#include "httplib.h"  // 使用 cpp-httplib 库
#include <mysqlx/xdevapi.h>  // 使用 mysqlx 协议

using ::std::cout;
using ::std::endl;
using namespace ::mysqlx;

int main() {
    httplib::Server svr;

    UserController userController;

    // 配置二级路由
    //  [&] 是Lambda 表达式的捕获列表
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

    // 数据库连接配置
    try {
        // 创建会话
        const char* url = "mysqlx://root:12345678@localhost:3306";
        cout << "Creating session on " << url << " ..." << endl;

        Session session(url);

        cout << "Session accepted, creating schema..." << endl;

        // 选择或创建数据库
        Schema db = session.getSchema("your_database_name");
        if (!db.existsInDatabase()) {
            db = session.createSchema("your_database_name");
        }

        cout << "Schema created or selected: " << db.getName() << endl;

        // 创建或选择集合
        Collection coll = db.createCollection("users", true);

        cout << "Collection created or selected: " << coll.getName() << endl;

        // 示例：执行查询
        mysqlx::Table table = db.getTable("your_table_name");
        mysqlx::RowResult result = table.select().execute();

        for (const mysqlx::Row &row : result) {
            cout << "Row: " << row[0] << ", " << row[1] << endl;
        }
    } catch (const mysqlx::Error &e) {
        cout << "MySQL Error: " << e.what() << endl;  // 使用 what() 方法获取错误信息
        return 1;
    } catch (const std::exception &e) {
        cout << "General Error: " << e.what() << endl;  // 捕获其他类型的异常
        return 1;
    } catch (const char *ex) {
        cout << "EXCEPTION: " << ex << endl;
        return 1;
    }

    cout << "Server started on http://localhost:8080" << endl;
    svr.listen("localhost", 8080);

    return 0;
}