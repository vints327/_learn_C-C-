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

void parseEthernetHeader(const u_char* data) {
    struct EthernetHeader {
        u_char dest[6];
        u_char src[6];
        u_short type;
    };

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
        printAndLog(std::to_string(i++) + ": " + device->name);
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