// main03.cpp
#include <iostream>
#include <stdexcept>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <filesystem> // 添加文件系统库
#include "httplib.h"

// 日志宏定义
#define LOG_INFO(msg) std::cout << "[INFO] " << msg << std::endl;
#define LOG_ERROR(msg) std::cerr << "[ERROR] " << msg << std::endl;

// 读取 .env 文件并设置环境变量
void loadEnvFile(const std::string &filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        LOG_ERROR("Failed to open .env file: " << filePath);
        return;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value))
        {
            // 去除前后空格
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            // 设置环境变量
#ifdef _WIN32
            _putenv_s(key.c_str(), value.c_str());
#else
            setenv(key.c_str(), value.c_str(), 1);
#endif
        }
    }
    file.close();
}

int main()
{
    try
    {
        // 获取项目根目录路径
        std::filesystem::path rootPath = std::filesystem::path(__FILE__).parent_path().parent_path();

        // 拼接 .env 文件路径
        std::string envFilePath = (rootPath / ".env").string();

        // 输出当前工作目录和 .env 文件路径
        LOG_INFO("Current working directory: " << std::filesystem::current_path().string());
        LOG_INFO("Loading .env file from: " << envFilePath);

        // 读取 .env 文件
        loadEnvFile(envFilePath);

        // 从环境变量中读取端口号和IP地址
        const char *ip = std::getenv("SERVER_IP") ? std::getenv("SERVER_IP") : "0.0.0.0";
        int port = std::getenv("SERVER_PORT") ? std::stoi(std::getenv("SERVER_PORT")) : 8080;

        LOG_INFO("Starting HTTP server on " << ip << ":" << port);

        httplib::Server svr;

        // 处理根路径 /
        svr.Get("/", [](const httplib::Request &req, httplib::Response &res)
                { res.set_content("Welcome to the default page!", "text/plain"); });

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