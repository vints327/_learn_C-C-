// main.cpp
#include <iostream>
// #include <httplib.h>
#include <stdexcept>
#include <string>
// #include "D:/vcpkg/installed/x64-windows/include/httplib.h"
#include "httplib.h"
// 日志宏定义
#define LOG_INFO(msg) std::cout << "[INFO] " << msg << std::endl;
#define LOG_ERROR(msg) std::cerr << "[ERROR] " << msg << std::endl;

int main()
{
    try
    {
        // 从环境变量或配置文件中读取端口号和IP地址
        const char *ip = std::getenv("SERVER_IP") ? std::getenv("SERVER_IP") : "0.0.0.0";
        int port = std::getenv("SERVER_PORT") ? std::stoi(std::getenv("SERVER_PORT")) : 8080;

        LOG_INFO("Starting HTTP server on " << ip << ":" << port);

        httplib::Server svr;

        // 处理 /hello 请求
        svr.Get("/hello", [](const httplib::Request &req, httplib::Response &res)
                { res.set_content("Hello, World!", "text/plain"); });

        // 处理 /greet 请求，增加输入验证
        svr.Get("/greet", [](const httplib::Request &req, httplib::Response &res)
                {
            auto name = req.get_param_value("name");

            // 输入验证
            if (name.empty()) {
                res.set_content("Please provide a name via ?name=YourName", "text/plain");
            } else if (name.length() > 100) { // 防止过长的输入
                res.set_content("Name is too long", "text/plain");
            } else if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ ") != std::string::npos) { // 防止特殊字符
                res.set_content("Invalid characters in name", "text/plain");
            } else {
                res.set_content("Hello, " + name + "!", "text/plain");
            } });

        // 启动服务器并捕获异常
        if (!svr.listen(ip, port))
        {
            throw std::runtime_error("Failed to start server on " + std::string(ip) + ":" + std::to_string(port));
        }
    }
    catch (const std::exception &e)
    {
        LOG_ERROR(e.what());
        return 1;
    }

    return 0;
}