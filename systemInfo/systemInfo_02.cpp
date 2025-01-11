#include <iostream>
#include <windows.h>
#include <psapi.h>
#include <string>

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

/**
 * 获取所有进程的 PID 并返回格式化的字符串
 */
std::wstring getAllProcessIDs()
{
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    unsigned int i;

    // 获取所有进程的 PID
    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
    {
        return L"Failed to get process IDs.";
    }

    // 计算进程数量
    cProcesses = cbNeeded / sizeof(DWORD);

    // 构建进程 ID 字符串
    std::wstring processIDs = L"Process IDs:\n";
    for (i = 0; i < cProcesses; i++)
    {
        if (aProcesses[i] != 0)
        {
            processIDs += std::to_wstring(aProcesses[i]) + L"\n";
        }
    }

    return processIDs;
}

/**
 * 创建可调整大小的消息框
 */
void createResizableMessageBox(const std::wstring &message, const std::wstring &title)
{
    MSGBOXPARAMSW msgboxParams = {0};
    msgboxParams.cbSize = sizeof(MSGBOXPARAMSW);
    msgboxParams.hwndOwner = NULL;
    msgboxParams.hInstance = NULL;
    msgboxParams.lpszText = message.c_str();
    msgboxParams.lpszCaption = title.c_str();
    msgboxParams.dwStyle = MB_OK | MB_USERICON | MB_SETFOREGROUND | MB_TASKMODAL | MB_TOPMOST | MB_ICONINFORMATION | MB_DEFBUTTON1;

    // 创建可调整大小的消息框
    MessageBoxIndirectW(&msgboxParams);
}

int main()
{
    // 调用函数获取并打印系统信息
    getSystemInfo();

    // 获取所有进程的 PID
    std::wstring processIDs = getAllProcessIDs();

    // 使用宽字符字符串，调用 MessageBoxW 函数
    MessageBoxW(NULL, L"Hello, Windows!", L"Message", MB_OK);

    // 显示进程 ID 消息框
    createResizableMessageBox(processIDs, L"Process IDs");

    // 等待用户输入，防止窗口一闪而过
    std::cout << "Press enter to exit..." << std::endl;
    std::cin.get();

    return 0;
}