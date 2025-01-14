// main.cpp
#include "Controller.h"

int main()
{
    try
    {
        Controller controller;
        controller.startServer();
    }
    catch (const std::exception &e)
    {
        std::cerr << "[ERROR] " << e.what() << std::endl;
        return 1;
    }

    return 0;
}