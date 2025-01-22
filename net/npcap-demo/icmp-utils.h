#ifndef ICMP_UTILS_H
#define ICMP_UTILS_H

#include <winsock2.h>
#include <pcap.h>
#include <string>

// ICMP 报文类型常量
#define ICMP_ECHOREPLY 0
#define ICMP_ECHO 8

// ICMP 报文结构
struct ICMPHeader {
    BYTE Type;        // ICMP 类型
    BYTE Code;        // ICMP 代码
    USHORT Checksum;  // 校验和
    USHORT Identifier; // 标识符
    USHORT Sequence;   // 序列号
};

// 计算校验和
USHORT CalculateChecksum(USHORT* buffer, int size);

// 发送 ICMP 请求
void send_icmp_request(pcap_t* adhandle, const std::string& ip, int sequence);

#endif // ICMP_UTILS_H