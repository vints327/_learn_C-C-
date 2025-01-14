// Controller.h
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Model.h"
#include "View.h"
#include <httplib.h>
#include <string>

class Controller
{
private:
    Model model;
    View view;
    httplib::Server svr;
    std::string ip;
    int port;

public:
    Controller();
    void startServer();
    void handleDefaultPage(const httplib::Request &req, httplib::Response &res);
    void handleHelloPage(const httplib::Request &req, httplib::Response &res);
    void handleGreetPage(const httplib::Request &req, httplib::Response &res);
};

#endif // CONTROLLER_H
