
// View.cpp
#include "View.h"

void View::logInfo(const std::string &msg)
{
    std::cout << "[INFO] " << msg << std::endl;
}

void View::logError(const std::string &msg)
{
    std::cerr << "[ERROR] " << msg << std::endl;
}

void View::sendResponse(httplib::Response &res, const std::string &content, const std::string &contentType)
{
    res.set_content(content, contentType);
}