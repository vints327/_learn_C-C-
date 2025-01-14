// Controller.h
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Model.h"
#include "View.h"
#include <httplib.h>
#include <string>

/**
 * Controller类负责模型、视图与HTTP服务器之间的交互
 * 它通过HTTP请求来调用模型中的数据处理逻辑，并通过视图呈现结果
 */
class Controller
{
private:
    Model model; // 模型对象，用于数据处理
    View view;   // 视图对象，用于结果展示
    httplib::Server svr; // HTTP服务器对象
    std::string ip;      // 服务器IP地址
    int port;            // 服务器端口号

public:
    /**
     * Controller构造函数
     * 初始化模型、视图和HTTP服务器
     */
    Controller();

    /**
     * 启动HTTP服务器
     * 绑定处理函数到特定的路由，并开始监听
     */
    void startServer();

    /**
     * 处理默认页面的请求
     * 当访问根路径时调用此函数
     * 
     * @param req HTTP请求对象，包含请求的所有信息
     * @param res HTTP响应对象，用于向客户端发送响应
     */
    void handleDefaultPage(const httplib::Request &req, httplib::Response &res);

    /**
     * 处理"hello"页面的请求
     * 当访问"/hello"路径时调用此函数
     * 
     * @param req HTTP请求对象
     * @param res HTTP响应对象
     */
    void handleHelloPage(const httplib::Request &req, httplib::Response &res);

    /**
     * 处理"greet"页面的请求
     * 当访问"/greet"路径时调用此函数
     * 
     * @param req HTTP请求对象
     * @param res HTTP响应对象
     */
    void handleGreetPage(const httplib::Request &req, httplib::Response &res);
};

#endif // CONTROLLER_H