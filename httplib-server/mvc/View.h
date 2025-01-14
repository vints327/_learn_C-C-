// View.h
#ifndef VIEW_H
#define VIEW_H

#include <string>
#include <iostream>
#include <httplib.h>

class View
{
public:
    void logInfo(const std::string &msg);
    void logError(const std::string &msg);
    void sendResponse(httplib::Response &res, const std::string &content, const std::string &contentType);
};

#endif // VIEW_H
