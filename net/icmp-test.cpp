// 包含必要的头文件
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <pcap.h>
#include <conio.h>  // 用于 _getch()
#include <fstream>   // 用于文件操作

// 链接必要的库
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "wpcap.lib")
#pragma comment(lib, "Packet.lib")

// 定义 ICMP 报文类型常量
#define ICMP_ECHOREPLY 0
#define ICMP_ECHO 8

// 定义一个互斥锁以保护多个线程同时访问 results 向量
std::mutex mtx;
// 存储 IP 地址和对应的活动状态
std::vector<std::pair<std::string, bool>> results;

// 条件变量和标志用于同步
std::condition_variable cv;
bool all_threads_done = false;

// ICMP 报文结构
struct ICMPHeader {
    BYTE Type;        // ICMP 类型
    BYTE Code;        // ICMP 代码
    USHORT Checksum;  // 校验和
    USHORT Identifier; // 标识符
    USHORT Sequence;   // 序列号
};

// 计算校验和
USHORT CalculateChecksum(USHORT* buffer, int size) {
    unsigned long cksum = 0;
    while (size > 1) {
        cksum += *buffer++;
        size -= sizeof(USHORT);
    }
    if (size) {
        cksum += *(UCHAR*)buffer;
    }
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);
    return (USHORT)(~cksum);
}

// 自定义 IP 头结构体
#pragma pack(push, 1) // 确保结构体按字节对齐
struct IPHeader {
    BYTE ihl : 4;      // IP 头长度 (4 bits)
    BYTE version : 4;  // 版本 (4 bits)
    BYTE tos;          // 服务类型
    USHORT total_len;  // 总长度
    USHORT id;         // 标识符
    USHORT frag_offset;// 分片偏移
    BYTE ttl;          // 生存时间
    BYTE protocol;     // 协议类型
    USHORT checksum;   // 校验和
    struct in_addr src_ip; // 源 IP 地址
    struct in_addr dest_ip; // 目标 IP 地址
};
#pragma pack(pop)

// TCP 和 UDP 头结构体
#pragma pack(push, 1) // 确保结构体按字节对齐
struct tcphdr {
    USHORT source;
    USHORT dest;
    ULONG seq;
    ULONG ack_seq;
    BYTE reserved1 : 4;
    BYTE th_off : 4;
    BYTE flags;
    BYTE window;
    USHORT check;
    USHORT urg_ptr;
};

struct udphdr {
    USHORT uh_sport;
    USHORT uh_dport;
    USHORT uh_ulen;
    USHORT uh_sum;
};
#pragma pack(pop)

// 处理接收到的数据包
std::ofstream logFile;

void packet_handler(u_char* param, const struct pcap_pkthdr* header, const u_char* pkt_data) {
    struct IPHeader* ip_header = (struct IPHeader*)(pkt_data + 14); // 跳过以太网帧头
    int ip_header_length = (ip_header->ihl & 0x0F) * 4;

    if (ip_header_length < 20) {
        std::cerr << "Invalid IP header length" << std::endl;
        return;
    }

    BYTE protocol = ip_header->protocol;

    if (protocol == IPPROTO_TCP || protocol == IPPROTO_UDP) {
        char src_ip_str[INET_ADDRSTRLEN];
        char dst_ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(ip_header->src_ip), src_ip_str, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(ip_header->dest_ip), dst_ip_str, INET_ADDRSTRLEN);

        std::string protocol_name = (protocol == IPPROTO_TCP) ? "TCP" : "UDP";
        std::string src_port_str, dst_port_str;

        if (protocol == IPPROTO_TCP) {
            struct tcphdr* tcp_header = (struct tcphdr*)((u_char*)ip_header + ip_header_length);
            src_port_str = std::to_string(ntohs(tcp_header->source));
            dst_port_str = std::to_string(ntohs(tcp_header->dest));
        } else if (protocol == IPPROTO_UDP) {
            struct udphdr* udp_header = (struct udphdr*)((u_char*)ip_header + ip_header_length);
            src_port_str = std::to_string(ntohs(udp_header->uh_sport));
            dst_port_str = std::to_string(ntohs(udp_header->uh_dport));
        }

        std::string packet_info = "Protocol: " + protocol_name + ", Src IP: " + src_ip_str + ", Src Port: " + src_port_str + ", Dst IP: " + dst_ip_str + ", Dst Port: " + dst_port_str;

        std::cout << packet_info << std::endl;
        if (logFile.is_open()) {
            logFile << packet_info << std::endl;
        }
    }
}

// 发送 ICMP 请求
void send_icmp_request(pcap_t* adhandle, const std::string& ip, int sequence) {
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &dest.sin_addr);

    struct ICMPHeader icmp_header;
    icmp_header.Type = ICMP_ECHO;
    icmp_header.Code = 0;
    icmp_header.Checksum = 0;
    icmp_header.Identifier = (USHORT)getpid();
    icmp_header.Sequence = (USHORT)sequence;

    char sendbuf[sizeof(ICMPHeader) + 32];
    memcpy(sendbuf, &icmp_header, sizeof(ICMPHeader));
    memset(sendbuf + sizeof(ICMPHeader), 'P', 32);  // 填充数据部分为 'P'

    icmp_header.Checksum = CalculateChecksum((USHORT*)sendbuf, sizeof(ICMPHeader) + 32);
    memcpy(sendbuf, &icmp_header, sizeof(ICMPHeader));

    if (pcap_sendpacket(adhandle, (u_char*)sendbuf, sizeof(ICMPHeader) + 32) != 0) {
        std::cerr << "发送数据包失败: " << pcap_geterr(adhandle) << std::endl;
    }
}

// 线程函数，ping 指定的 IP 地址
void ping_ip(const std::string& ip, pcap_t* adhandle, int sequence) {
    send_icmp_request(adhandle, ip, sequence);
}

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
        std::vector<std::thread> threads;

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
            threads.emplace_back(ping_ip, ip, adhandle, i);
        }

        // 启动捕获线程
        std::thread capture_thread([adhandle]() {
            pcap_loop(adhandle, 0, packet_handler, NULL);
        });

        // 等待所有线程完成
        for (auto& th : threads) {
            th.join();
        }

        // 停止捕获线程
        pcap_breakloop(adhandle);
        capture_thread.join();

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