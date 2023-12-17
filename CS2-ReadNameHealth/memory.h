#include<Windows.h>
#include <Tlhelp32.h>
#include <iostream>
#include <thread>
#include<string>
using namespace std;
class Memory {
public:
    HWND hWnd;
    HANDLE processHandle;
    HANDLE snapShot;
    MODULEENTRY32 entry;

    Memory() {}

    DWORD GetProcessID(const wchar_t* windowName) {
        DWORD processID;
        do {
            hWnd = FindWindow(NULL, windowName);
            Sleep(50);
            if (hWnd) break;
            cout << "未找到CS2窗口,请先启动CS2\n";
        } while (!hWnd);
        GetWindowThreadProcessId(hWnd, &processID);
        cout << "CS2 ProcessID: " << processID << endl;
        processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
        cout << "CS2 processHandle: " << processHandle << endl;
        return processID;
    }

    MODULEENTRY32 GetModuleAddress(DWORD processID, const wchar_t* moduleName) {
        snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);
        entry.dwSize = sizeof(MODULEENTRY32);
        if (snapShot != INVALID_HANDLE_VALUE) {
            if (Module32First(snapShot, &entry)) {
                do {
                    if (!wcscmp((const wchar_t*)entry.szModule, moduleName)) {
                        break;
                    }
                } while (Module32Next(snapShot, &entry));
            }
            if (snapShot)
                CloseHandle(snapShot);
        }
        cout << "CS2 SnapShot: " << snapShot << endl;
        this_thread::sleep_for(chrono::seconds(1));
        return entry;
    }

    template <typename T>
    T ReadMemory(DWORD64 address) {
        T buffer;
        ReadProcessMemory(processHandle, (LPCVOID)address, &buffer, sizeof(buffer), NULL);
        return buffer;
    }

    template <typename T>
    void WriteMemory(DWORD64 address, T val) {
        WriteProcessMemory(processHandle, (LPVOID)address, &val, sizeof(val), NULL);
    }
};