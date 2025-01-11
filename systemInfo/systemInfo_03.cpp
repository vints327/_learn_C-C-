#include <iostream>
#include <windows.h>
#include <psapi.h>
#include <string>

#define IDD_DIALOG1 101

#ifndef IDC_EDIT1
#define IDC_EDIT1 1001 // 假设 1001 是编辑控件的 ID
#endif

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
 * 对话框过程函数
 */
INT_PTR CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        HWND hEdit = GetDlgItem(hwndDlg, IDC_EDIT1);
        std::wstring processIDs = getAllProcessIDs();
        SetWindowTextW(hEdit, processIDs.c_str());
        return (INT_PTR)TRUE;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hwndDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    // 调用函数获取并打印系统信息
    getSystemInfo();

    // 显示自定义对话框
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);

    // 等待用户输入，防止窗口一闪而过
    std::cout << "Press enter to exit..." << std::endl;
    std::cin.get();

    return 0;
}