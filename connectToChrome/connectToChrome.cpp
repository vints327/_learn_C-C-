#include <cstdlib>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/client.hpp>
#include <boost/asio.hpp> // For checking Chrome readiness

typedef websocketpp::client<websocketpp::config::asio_no_tls> client;

void startChrome()
{
    std::string command = "start chrome --remote-debugging-port=9222";
    system(command.c_str());
}

bool isChromeReady()
{
    try
    {
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("localhost", "9222");
        boost::asio::ip::tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

void connectToChrome()
{
    client c;
    c.init_asio();

    c.set_message_handler([](websocketpp::connection_hdl hdl, client::message_ptr msg)
                          { std::cout << "Received: " << msg->get_payload() << std::endl; });

    websocketpp::lib::error_code ec;

    // Example connection URL, update based on Chrome DevTools JSON
    std::string ws_url = "ws://localhost:9222/devtools/browser/<unique-id>";
    client::connection_ptr con = c.get_connection(ws_url, ec);

    if (ec)
    {
        std::cerr << "Could not create connection: " << ec.message() << std::endl;
        return;
    }

    try
    {
        c.connect(con);
        c.run();
    }
    catch (const websocketpp::exception &e)
    {
        std::cerr << "WebSocket exception: " << e.what() << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Standard exception: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown error occurred." << std::endl;
    }
}

int main()
{
    std::thread chromeThread(startChrome);

    // Wait for Chrome to be ready
    while (!isChromeReady())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::thread websocketThread(connectToChrome);

    chromeThread.join();
    websocketThread.join();

    return 0;
}
