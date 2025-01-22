#ifndef PACKET_HANDLER_H
#define PACKET_HANDLER_H

#include <pcap.h>
#include <string>
#include <fstream>

// 声明 logFile 变量为 extern
extern std::ofstream logFile;

// 处理接收到的数据包
void packet_handler(u_char* param, const struct pcap_pkthdr* header, const u_char* pkt_data);

#endif // PACKET_HANDLER_H