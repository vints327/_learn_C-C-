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
#include <conio.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wpcap.lib")

std::mutex logMutex;
std::ofstream logFile("packet_log.txt", std::ios::out);

void printAndLog(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::cout << message << std::endl;
    if (logFile.is_open()) {
        logFile << message << std::endl;
    }
}

struct EthernetHeader {
    u_char dest[6];
    u_char src[6];
    u_short type;
};

struct IPHeader {
    u_char versionAndHeaderLength;
    u_char tos;
    u_short totalLength;
    u_short id;
    u_short flagsAndOffset;
    u_char ttl;
    u_char protocol;
    u_short checksum;
    u_int srcAddr;
    u_int destAddr;
};

struct TCPHeader {
    u_short srcPort;
    u_short destPort;
    u_int seqNum;
    u_int ackNum;
    u_char offsetAndReserved;
    u_char flags;
    u_short window;
    u_short checksum;
    u_short urgentPtr;
};

struct UDPHeader {
    u_short srcPort;
    u_short destPort;
    u_short length;
    u_short checksum;
};

void parseEthernetHeader(const u_char* data) {
    const EthernetHeader* eth = reinterpret_cast<const EthernetHeader*>(data);

    std::ostringstream ss;
    ss << "Ethernet Header:";
    ss << "\n  Destination MAC: ";
    for (int i = 0; i < 6; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)eth->dest[i];
        if (i < 5) ss << ":";
    }
    ss << "\n  Source MAC: ";
    for (int i = 0; i < 6; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)eth->src[i];
        if (i < 5) ss << ":";
    }
    ss << "\n  Type: " << std::hex << ntohs(eth->type);
    printAndLog(ss.str());
}

void parseIPHeader(const u_char* data) {
    const IPHeader* ip = reinterpret_cast<const IPHeader*>(data);

    int headerLength = (ip->versionAndHeaderLength & 0x0F) * 4;
    std::ostringstream ss;
    ss << "IP Header:";
    ss << "\n  Version: " << ((ip->versionAndHeaderLength >> 4) & 0x0F);
    ss << "\n  Header Length: " << headerLength << " bytes";
    ss << "\n  Total Length: " << ntohs(ip->totalLength);
    ss << "\n  Protocol: " << (int)ip->protocol;
    ss << "\n  Source IP: " << inet_ntoa(*(in_addr*)&ip->srcAddr);
    ss << "\n  Destination IP: " << inet_ntoa(*(in_addr*)&ip->destAddr);
    printAndLog(ss.str());
}

void packetHandler(u_char* param, const struct pcap_pkthdr* header, const u_char* pktData) {
    if (header->caplen < 14) return; // Ensure minimum Ethernet header size

    parseEthernetHeader(pktData);

    if (header->caplen > 14) {
        parseIPHeader(pktData + 14); // Ethernet header is 14 bytes

        const IPHeader* ip = reinterpret_cast<const IPHeader*>(pktData + 14);
        int ipHeaderLength = (ip->versionAndHeaderLength & 0x0F) * 4;
        int transportHeaderOffset = 14 + ipHeaderLength;

        if (header->caplen > transportHeaderOffset) {
            switch (ip->protocol) {
                case IPPROTO_TCP: {
                    const TCPHeader* tcp = reinterpret_cast<const TCPHeader*>(pktData + transportHeaderOffset);
                    std::ostringstream ss;
                    ss << "TCP Header:";
                    ss << "\n  Source Port: " << ntohs(tcp->srcPort);
                    ss << "\n  Destination Port: " << ntohs(tcp->destPort);
                    ss << "\n  Sequence Number: " << ntohl(tcp->seqNum);
                    ss << "\n  Acknowledgment Number: " << ntohl(tcp->ackNum);
                    ss << "\n  Data Offset: " << ((tcp->offsetAndReserved >> 4) * 4) << " bytes";
                    ss << "\n  Flags: ";
                    if (tcp->flags & 0x01) ss << "FIN ";
                    if (tcp->flags & 0x02) ss << "SYN ";
                    if (tcp->flags & 0x04) ss << "RST ";
                    if (tcp->flags & 0x08) ss << "PSH ";
                    if (tcp->flags & 0x10) ss << "ACK ";
                    if (tcp->flags & 0x20) ss << "URG ";
                    ss << "\n  Window Size: " << ntohs(tcp->window);
                    ss << "\n  Checksum: " << ntohs(tcp->checksum);
                    ss << "\n  Urgent Pointer: " << ntohs(tcp->urgentPtr);
                    printAndLog(ss.str());
                    break;
                }
                case IPPROTO_UDP: {
                    const UDPHeader* udp = reinterpret_cast<const UDPHeader*>(pktData + transportHeaderOffset);
                    std::ostringstream ss;
                    ss << "UDP Header:";
                    ss << "\n  Source Port: " << ntohs(udp->srcPort);
                    ss << "\n  Destination Port: " << ntohs(udp->destPort);
                    ss << "\n  Length: " << ntohs(udp->length);
                    ss << "\n  Checksum: " << ntohs(udp->checksum);
                    printAndLog(ss.str());
                    break;
                }
                default:
                    printAndLog("Unsupported protocol: " + std::to_string(ip->protocol));
                    break;
            }
        }
    }
}

void startPacketCapture(pcap_if_t* device) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* handle = pcap_open_live(device->name, 65536, 1, 1000, errbuf);
    if (!handle) {
        printAndLog("Failed to open device for capture: " + std::string(errbuf));
        return;
    }

    printAndLog("Starting packet capture on device: " + std::string(device->name));

    pcap_loop(handle, 0, packetHandler, nullptr);

    pcap_close(handle);
}

int main() {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t* allDevices;

    if (pcap_findalldevs(&allDevices, errbuf) == -1) {
        printAndLog("Error finding devices: " + std::string(errbuf));
        return 1;
    }

    printAndLog("Available Devices:");
    pcap_if_t* device;
    int i = 0;
    for (device = allDevices; device != nullptr; device = device->next) {
        std::string desc = device->description ? device->description : "No description available";
        printAndLog(std::to_string(i++) + ": " + device->name + " (" + desc + ")");
    }

    printAndLog("\nSelect a device by index: ");
    int deviceIndex;
    std::cin >> deviceIndex;

    device = allDevices;
    for (int j = 0; j < deviceIndex && device != nullptr; ++j) {
        device = device->next;
    }

    if (!device) {
        printAndLog("Invalid device index.");
        pcap_freealldevs(allDevices);
        return 1;
    }

    std::thread captureThread(startPacketCapture, device);

    printAndLog("Press any key to stop capturing...");
    _getch();

    pcap_breakloop(nullptr);
    captureThread.join();

    pcap_freealldevs(allDevices);
    logFile.close();

    return 0;
}