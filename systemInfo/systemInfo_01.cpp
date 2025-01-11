#include <iostream>
#include <windows.h>
#include <psapi.h>

/**
 * 获取系统信息，包括处理器数量和物理内存情况
 */
void getSystemInfo()
{
    // 初始化系统信息结构体
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    // 输出处理器数量
    std::cout << "Number of processors: " << sysInfo.dwNumberOfProcessors << std::endl;

    // 初始化内存状态结构体
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    // 输出总物理内存
    std::cout << "Total physical memory: " << memInfo.ullTotalPhys / (1024 * 1024) << " MB" << std::endl;
    // 输出可用物理内存
    std::cout << "Available physical memory: " << memInfo.ullAvailPhys / (1024 * 1024) << " MB" << std::endl;
}

int main()
{
    // 调用函数获取并打印系统信息
    getSystemInfo();

    // 使用宽字符字符串，调用 MessageBoxW 函数
    MessageBoxW(NULL, L"Hello, Windows!", L"Message", MB_OK);

    // 等待用户输入，防止窗口一闪而过
    std::cout << "Press enter to exit..." << std::endl;
    std::cin.get();

    return 0;
}