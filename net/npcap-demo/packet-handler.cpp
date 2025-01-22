#include "packet-handler.h"
#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <fstream>   // 用于文件操作

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

// 声明 logFile 变量
// std::ofstream logFile;

// 处理接收到的数据包
void packet_handler(u_char* param, const struct pcap_pkthdr* header, const u_char* pkt_data) {
    // 打印时间戳和数据包长度
    std::cout << "时间戳: " << header->ts.tv_sec << "." << header->ts.tv_usec << " 秒" << std::endl;
    std::cout << "数据包长度: " << header->len << " 字节" << std::endl;

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

        std::string packet_info = "协议: " + protocol_name + ", 源IP: " + src_ip_str + ", 源端口: " + src_port_str + ", 目标IP: " + dst_ip_str + ", 目标端口: " + dst_port_str;

        std::cout << packet_info << std::endl;
        if (logFile.is_open()) {
            logFile << packet_info << std::endl;
        }
    }
}