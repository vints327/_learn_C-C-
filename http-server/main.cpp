// main.cpp
#include <iostream>
// #include <httplib.h>
#include "httplib.h" // 修改为直接包含 httplib.h
// #include "cpp-httplib/cpp-httplib.h"

int main()
{
    std::cout << "Starting HTTP server on port 8080..." << std::endl;

    httplib::Server svr;

    svr.Get("/hello", [](const httplib::Request &req, httplib::Response &res)
            { res.set_content("Hello, World!", "text/plain"); });

    svr.Get("/greet", [](const httplib::Request &req, httplib::Response &res)
            {
        auto name = req.get_param_value("name");
        if (name.empty()) {
            res.set_content("Please provide a name via ?name=YourName", "text/plain");
        } else {
            res.set_content("Hello, " + name + "!", "text/plain");
        } });

    svr.listen("0.0.0.0", 8080);

    return 0;
}