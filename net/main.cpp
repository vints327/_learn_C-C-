#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

// 链接必要的库
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

// 定义一个互斥锁以保护多个线程同时访问 results 向量
std::mutex mtx;
// 存储 IP 地址和对应的主机名对
std::vector<std::pair<std::string, std::string>> results;

/**
 * 扫描指定的 IP 地址，判断其是否活跃并解析主机名。
 * @param ip 要扫描的 IP 地址。
 */
void scan_ip(const std::string& ip) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup 失败: " << WSAGetLastError() << std::endl;
        return;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "创建套接字失败: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(80); // 尝试连接到端口 80
    inet_pton(AF_INET, ip.c_str(), &server.sin_addr);

    // 如果连接成功，尝试解析主机名
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) == 0) {
        char hostname[NI_MAXHOST];
        if (getnameinfo((struct sockaddr*)&server, sizeof(server), hostname, NI_MAXHOST, NULL, 0, NI_NAMEREQD) == 0) {
            mtx.lock();
            results.push_back({ip, hostname});
            mtx.unlock();
        } else {
            mtx.lock();
            results.push_back({ip, "未知"});
            mtx.unlock();
        }
    }

    closesocket(sock);
    WSACleanup();
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup 失败: " << WSAGetLastError() << std::endl;
        return 1;
    }

    const std::string base_ip = "192.168.31.";
    std::vector<std::thread> threads;

    // 创建线程池，每个线程负责扫描一个 IP 地址
    for (int i = 1; i <= 254; ++i) {
        std::string ip = base_ip + std::to_string(i);
        threads.emplace_back(scan_ip, ip);
    }

    // 等待所有线程完成
    for (auto& th : threads) {
        th.join();
    }

    std::cout << "网络中的活动主机:\n";
    for (const auto& result : results) {
        std::cout << "IP: " << result.first << " - 主机名: " << result.second << "\n";
    }

    WSACleanup();
    return 0;
}