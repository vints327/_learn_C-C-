#include "icmp-utils.h"
#include <iostream>
#include <string>
#include <cstring>
#include <mutex>  // 添加互斥锁头文件
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>  // 包含 getpid() 函数的头文件
#endif

// 定义一个互斥锁以保护多个线程同时访问控制台输出
std::mutex mtx;

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

// 发送 ICMP 请求
/**
 * 发送ICMP请求数据包
 * 
 * @param adhandle pcap打开的适配器句柄，用于发送数据包
 * @param ip 目标主机的IP地址，以字符串形式表示
 * @param sequence ICMP回声请求的序列号
 * 
 * 此函数负责构造ICMP回声请求数据包，并通过指定的网络适配器发送出去。
 * 它首先设置目标IP地址，然后构造ICMP头部，包括类型、代码、校验和、标识符和序列号。
 * 随后，它填充数据部分，并计算整个数据包的校验和。最后，它通过pcap库发送构造好的数据包。
 */
void send_icmp_request(pcap_t* adhandle, const std::string& ip, int sequence) {
    // 检查适配器句柄是否有效
    if (adhandle == NULL) {
        std::cerr << "适配器句柄无效" << std::endl;
        return;
    }

    // 初始化目标IP地址
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &dest.sin_addr);

    // 构造ICMP头部
    struct ICMPHeader icmp_header;
    icmp_header.Type = ICMP_ECHO;
    icmp_header.Code = 0;
    icmp_header.Checksum = 0;
#ifdef _WIN32
    // 在Windows平台上，使用当前进程ID作为标识符
    icmp_header.Identifier = (USHORT)GetCurrentProcessId();
#else
    // 在非Windows平台上，使用当前进程ID作为标识符
    icmp_header.Identifier = (USHORT)getpid();
#endif
    // 设置序列号
    icmp_header.Sequence = (USHORT)sequence;

    // 准备发送缓冲区，大小为ICMP头部加上数据部分
    char sendbuf[sizeof(ICMPHeader) + 32];
    memcpy(sendbuf, &icmp_header, sizeof(ICMPHeader));
    memset(sendbuf + sizeof(ICMPHeader), 'P', 32);  // 填充数据部分为 'P'

    // 计算整个数据包的校验和
    icmp_header.Checksum = CalculateChecksum((USHORT*)sendbuf, sizeof(ICMPHeader) + 32);
    memcpy(sendbuf, &icmp_header, sizeof(ICMPHeader));

    // 添加互斥锁保护控制台输出
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "目标IP地址设置为: " << ip << std::endl;
    std::cout << "设置序列号为: " << icmp_header.Sequence << std::endl;
    std::cout << "计算得到的校验和为: " << icmp_header.Checksum << std::endl;

    // 发送数据包，如果失败则输出错误信息
    if (pcap_sendpacket(adhandle, (u_char*)sendbuf, sizeof(ICMPHeader) + 32) != 0) {
        std::cerr << "发送数据包失败: " << pcap_geterr(adhandle) << std::endl;
    } else {
        std::cout << "数据包发送成功" << std::endl;
    }

    // 强制刷新输出缓冲区
    std::cout.flush();
}