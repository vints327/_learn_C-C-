#include <iostream>
#include <string>
#include <vector>

#include <thread>  // 使用 std::thread 而不是 std::jthread
#include <mutex>
#include <condition_variable>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <pcap.h>
#include <conio.h>  // 用于 _getch()
#include <fstream>   // 用于文件操作

#include "icmp-utils.h"
#include "packet-handler.h"

// 链接必要的库
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "wpcap.lib")
#pragma comment(lib, "Packet.lib")

// 定义一个互斥锁以保护多个线程同时访问 results 向量
// std::mutex mtx;
// 存储 IP 地址和对应的活动状态
std::vector<std::pair<std::string, bool>> results;

// 条件变量和标志用于同步
std::condition_variable cv;
bool all_threads_done = false;

// 线程函数，ping 指定的 IP 地址
void ping_ip(const std::string& ip, pcap_t* adhandle, int sequence) {
    try {
        send_icmp_request(adhandle, ip, sequence);
    } catch (const std::exception& e) {
        std::cerr << "ping_ip 异常: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "ping_ip 未知异常" << std::endl;
    }
}

// 声明 logFile 变量
std::ofstream logFile;

int main() {
    // 设置控制台编码为 UTF-8
    SetConsoleOutputCP(CP_UTF8);

    try {
        // 初始化Winsock库
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup 失败: " << WSAGetLastError() << std::endl;
            return 1;
        }

        // 打开日志文件
        logFile.open("packet.log.txt", std::ios::out);
        if (!logFile.is_open()) {
            std::cerr << "无法打开 packet.log.txt 文件" << std::endl;
            WSACleanup();
            return 1;
        }

        const std::string base_ip = "192.168.31.";
        std::vector<std::thread> threads;  // 使用 std::thread

        // 打开适配器
        char errbuf[PCAP_ERRBUF_SIZE];
        pcap_if_t* alldevs;
        if (pcap_findalldevs(&alldevs, errbuf) == -1) {
            std::cerr << "查找设备失败: " << errbuf << std::endl;
            WSACleanup();
            logFile.close();
            return 1;
        }

        // 列出所有适配器供用户选择
        std::cout << "可用适配器列表:" << std::endl;
        int i = 0;
        for (pcap_if_t* d = alldevs; d != NULL; d = d->next) {
            std::cout << i << ": " << d->name;
            if (d->description) {
                std::cout << " (" << d->description << ")";
            }
            std::cout << std::endl;
            i++;
        }

        int choice;
        std::cout << "请选择适配器编号: ";
        std::cin >> choice;

        pcap_if_t* d = alldevs;
        for (int j = 0; j < choice && d != NULL; j++) {
            d = d->next;
        }

        if (d == NULL) {
            std::cerr << "无效的适配器编号" << std::endl;
            pcap_freealldevs(alldevs);
            WSACleanup();
            logFile.close();
            return 1;
        }

        // 打开指定的网络适配器
        pcap_t* adhandle = pcap_open_live(d->name, 65536, 1, 1000, errbuf);
        if (adhandle == NULL) {
            std::cerr << "打开适配器失败: " << errbuf << std::endl;
            pcap_freealldevs(alldevs);
            WSACleanup();
            logFile.close();
            return 1;
        }

        // 设置过滤器
        struct bpf_program fp;
        char filter_exp[] = "ip"; // 捕获所有IP数据包
        std::cout << "正在编译过滤器: " << filter_exp << std::endl;
        if (pcap_compile(adhandle, &fp, filter_exp, 0, 0) == -1) {
            std::cerr << "编译过滤器失败: " << pcap_geterr(adhandle) << std::endl;
            pcap_close(adhandle);
            pcap_freealldevs(alldevs);
            WSACleanup();
            logFile.close();
            return 1;
        }

        if (pcap_setfilter(adhandle, &fp) == -1) {
            std::cerr << "设置过滤器失败: " << pcap_geterr(adhandle) << std::endl;
            pcap_freecode(&fp);
            pcap_close(adhandle);
            pcap_freealldevs(alldevs);
            WSACleanup();
            logFile.close();
            return 1;
        }

        pcap_freecode(&fp);

        // 创建线程池，每个线程负责 ping 一个 IP 地址
        for (int i = 1; i <= 254; ++i) {
            std::string ip = base_ip + std::to_string(i);
            if (ip.find('.') == std::string::npos) {
                throw std::invalid_argument("Invalid base IP format");
            }
            threads.emplace_back(ping_ip, ip, adhandle, i);
        }

        // 启动捕获线程
        std::thread capture_thread([adhandle]() {
            pcap_loop(adhandle, 0, packet_handler, NULL);
        });

        // 等待所有线程完成
        try {
            for (auto& th : threads) {
                if (th.joinable()) {
                    th.join();
                }
            }
        } catch (...) {
            pcap_breakloop(adhandle);
            throw;
        }

        // 停止捕获线程
        pcap_breakloop(adhandle);
        if (capture_thread.joinable()) {
            capture_thread.join();
        }

        std::cout << "网络中的活动主机:\n";
        for (const auto& result : results) {
            if (result.second) {
                std::cout << "IP: " << result.first << " - 状态: 活跃\n";
            } else {
                std::cout << "IP: " << result.first << " - 状态: 不活跃\n";
            }
        }

        pcap_close(adhandle);
        pcap_freealldevs(alldevs);
        WSACleanup();
    } catch (const std::exception& e) {
        std::cerr << "捕获到异常: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "捕获到未知异常" << std::endl;
    }

    // 关闭日志文件
    if (logFile.is_open()) {
        logFile.close();
    }

    // 等待用户输入，以便查看输出结果
    std::cout << "按任意键继续..." << std::endl;
    _getch();

    return 0;
}