#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <icmpapi.h>

// 链接必要的库
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "icmp.lib")

// 定义一个互斥锁以保护多个线程同时访问 results 向量
std::mutex mtx;
// 存储 IP 地址和对应的活动状态
std::vector<std::pair<std::string, bool>> results;

/**
 * 使用 ICMP 协议 ping 指定的 IP 地址，判断其是否活跃。
 * @param ip 要 ping 的 IP 地址。
 */
void ping_ip(const std::string& ip) {
    HANDLE hIcmpFile = IcmpCreateFile();
    if (hIcmpFile == INVALID_HANDLE_VALUE) {
        std::cerr << "IcmpCreateFile 失败: " << GetLastError() << std::endl;
        return;
    }

    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &dest.sin_addr);

    unsigned long ipaddr = dest.sin_addr.S_un.S_addr;
    char sendbuf[32] = "Ping";
    DWORD replySize = sizeof(ICMP_ECHO_REPLY) + sizeof(sendbuf) + 8;
    char replybuf[replySize];

    DWORD reply = IcmpSendEcho(
        hIcmpFile,       // Icmp handle
        ipaddr,          // Target IP address
        sendbuf,         // Request data
        sizeof(sendbuf), // Request size
        NULL,            // No RequestOptions structure
        replybuf,        // Reply buffer
        replySize,       // Reply size
        1000             // Timeout
    );

    if (reply == 0) {
        DWORD err = GetLastError();
        std::cerr << "IcmpSendEcho 失败: " << err << std::endl;
    } else {
        PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)replybuf;
        if (pEchoReply->Status == IP_SUCCESS) {
            mtx.lock();
            results.push_back({ip, true});
            mtx.unlock();
        } else {
            mtx.lock();
            results.push_back({ip, false});
            mtx.unlock();
        }
    }

    IcmpCloseHandle(hIcmpFile);
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup 失败: " << WSAGetLastError() << std::endl;
        return 1;
    }

    const std::string base_ip = "192.168.31.";
    std::vector<std::thread> threads;

    // 创建线程池，每个线程负责 ping 一个 IP 地址
    for (int i = 1; i <= 254; ++i) {
        std::string ip = base_ip + std::to_string(i);
        threads.emplace_back(ping_ip, ip);
    }

    // 等待所有线程完成
    for (auto& th : threads) {
        th.join();
    }

    std::cout << "网络中的活动主机:\n";
    for (const auto& result : results) {
        if (result.second) {
            std::cout << "IP: " << result.first << " - 状态: 活跃\n";
        } else {
            std::cout << "IP: " << result.first << " - 状态: 不活跃\n";
        }
    }

    WSACleanup();
    return 0;
}