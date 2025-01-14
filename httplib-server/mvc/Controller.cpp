
// Controller.cpp
#include "Controller.h"
#include <filesystem>

Controller::Controller()
{
    // 获取项目根目录路径
    std::filesystem::path rootPath = std::filesystem::path(__FILE__).parent_path().parent_path();

    // 拼接 .env 文件路径
    std::string envFilePath = (rootPath / ".env").string();

    // 输出当前工作目录和 .env 文件路径
    view.logInfo("Current working directory: " + std::filesystem::current_path().string());
    view.logInfo("Loading .env file from: " + envFilePath);

    // 读取 .env 文件
    model.loadEnvFile(envFilePath);

    // 从环境变量中读取端口号和IP地址
    ip = model.getEnvVariable("SERVER_IP", "0.0.0.0");
    port = std::stoi(model.getEnvVariable("SERVER_PORT", "8080"));

    view.logInfo("Starting HTTP server on " + ip + ":" + std::to_string(port));

    // 设置路由处理函数
    svr.Get("/", [this](const httplib::Request &req, httplib::Response &res)
            { handleDefaultPage(req, res); });

    svr.Get("/hello", [this](const httplib::Request &req, httplib::Response &res)
            { handleHelloPage(req, res); });

    svr.Get("/greet", [this](const httplib::Request &req, httplib::Response &res)
            { handleGreetPage(req, res); });
}

void Controller::startServer()
{
    if (!svr.listen(ip, port))
    {
        view.logError("Failed to start server on " + ip + ":" + std::to_string(port));
        throw std::runtime_error("Failed to start server");
    }
}

void Controller::handleDefaultPage(const httplib::Request &req, httplib::Response &res)
{
    view.sendResponse(res, "Welcome to the default page!", "text/plain");
}

void Controller::handleHelloPage(const httplib::Request &req, httplib::Response &res)
{
    view.sendResponse(res, "Hello, World!", "text/plain");
}

void Controller::handleGreetPage(const httplib::Request &req, httplib::Response &res)
{
    auto name = req.get_param_value("name");

    // 输入验证
    if (name.empty())
    {
        view.sendResponse(res, "Please provide a name via ?name=YourName", "text/plain");
    }
    else if (name.length() > 100)
    { // 防止过长的输入
        view.sendResponse(res, "Name is too long", "text/plain");
    }
    else if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ ") != std::string::npos)
    { // 防止特殊字符
        view.sendResponse(res, "Invalid characters in name", "text/plain");
    }
    else
    {
        view.sendResponse(res, "Hello, " + name + "!", "text/plain");
    }
}