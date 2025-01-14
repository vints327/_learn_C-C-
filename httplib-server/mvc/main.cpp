// main.cpp
#include "Controller.h"

// 程序入口点
int main()
{
    try
    {
        // 创建Controller实例
        Controller controller;
        // 启动服务器
        controller.startServer();
    }
    catch (const std::exception &e)
    {
        // 捕获并打印异常信息
        std::cerr << "[ERROR] " << e.what() << std::endl;
        // 异常处理后返回错误码
        return 1;
    }

    // 程序正常结束返回0
    return 0;
}