
#include "pid.h"
#include <tlhelp32.h>
#include <wchar.h>

DWORD GetPIDByProcessNameA(LPCSTR lpszProcessName) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(snapshot, &entry)) {
        do {
            if (strcmp(entry.szExeFile, lpszProcessName) == 0) {
                CloseHandle(snapshot);
                return entry.th32ProcessID;
            }
        } while (Process32Next(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return 0;
}

DWORD GetPIDByProcessNameW(LPCWSTR lpwzProcessName) {
    PROCESSENTRY32W entry;
    entry.dwSize = sizeof(PROCESSENTRY32W);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32FirstW(snapshot, &entry)) {
        do {
            if (wcscmp(entry.szExeFile, lpwzProcessName) == 0) {
                CloseHandle(snapshot);
                return entry.th32ProcessID;
            }
        } while (Process32NextW(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return 0;
}

DWORD GetPIDByWindowTitleA(LPCSTR lpszWindowTitle) {
    HWND hwnd = FindWindowA(NULL, lpszWindowTitle);

    if (hwnd != NULL) {
        DWORD pid;
        GetWindowThreadProcessId(hwnd, &pid);
        return pid;
    }

    return 0;
}

DWORD GetPIDByWindowTitleW(LPCWSTR lpwzWindowTitle) {
    HWND hwnd = FindWindowW(NULL, lpwzWindowTitle);

    if (hwnd != NULL) {
        DWORD pid;
        GetWindowThreadProcessId(hwnd, &pid);
        return pid;
    }

    return 0;
}
