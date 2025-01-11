#include "httplib.h" // 确保 cpp-httplib 的 httplib.h 文件存在

int main()
{
    // 创建一个 HTTP 服务实例
    httplib::Server server;

    // 定义一个简单的路由处理器，返回 "Hello, World!" 文本
    server.Get("/", [](const httplib::Request &req, httplib::Response &res)
               { res.set_content("Hello, World!", "text/plain"); });

    // 定义一个动态路由处理器，处理用户请求时返回动态数据
    server.Get("/greet", [](const httplib::Request &req, httplib::Response &res)
               {
        std::string name = req.get_param_value("name", "Guest");
        res.set_content("Hello, " + name + "!", "text/plain"); });

    // 在 8080 端口上启动服务器
    std::cout << "Starting server at http://localhost:8080" << std::endl;
    server.listen("0.0.0.0", 8080);

    return 0;
}
