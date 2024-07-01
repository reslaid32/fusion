#include "inject.h"
#include <stdio.h>
#include <wchar.h>

BOOL LoadDLLA(DWORD dwProcessId, LPCSTR lpszDllPath) {
    HANDLE hProcess;
    LPVOID lpRemoteBuffer;
    DWORDLONG dwSize = strlen(lpszDllPath) + 1;
    DWORD dwThreadId;
    HANDLE hThread;

    hProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, dwProcessId);
    if (hProcess == NULL) {
        printf("Failed to open process. Error code: %lu\n", GetLastError());
        return FALSE;
    }

    lpRemoteBuffer = VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (lpRemoteBuffer == NULL) {
        printf("Failed to allocate memory in remote process. Error code: %lu\n", GetLastError());
        CloseHandle(hProcess);
        return FALSE;
    }

    if (!WriteProcessMemory(hProcess, lpRemoteBuffer, lpszDllPath, dwSize, NULL)) {
        printf("Failed to write to remote process memory. Error code: %lu\n", GetLastError());
        VirtualFreeEx(hProcess, lpRemoteBuffer, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }

    hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, lpRemoteBuffer, 0, &dwThreadId);
    if (hThread == NULL) {
        printf("Failed to create remote thread. Error code: %lu\n", GetLastError());
        VirtualFreeEx(hProcess, lpRemoteBuffer, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }

    WaitForSingleObject(hThread, INFINITE);

    CloseHandle(hThread);
    VirtualFreeEx(hProcess, lpRemoteBuffer, 0, MEM_RELEASE);
    CloseHandle(hProcess);

    return TRUE;
}

BOOL LoadDLLW(DWORD dwProcessId, LPCWSTR lpwzDllPath) {
    HANDLE hProcess;
    LPVOID lpRemoteBuffer;
    DWORDLONG dwSize = wcslen(lpwzDllPath) * sizeof(wchar_t) + sizeof(wchar_t);
    DWORD dwThreadId;
    HANDLE hThread;

    hProcess = OpenProcess(PROCESS_ALL_ACCESS | PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, dwProcessId);
    if (hProcess == NULL) {
        wprintf(L"Failed to open process. Error code: %lu\n", GetLastError());
        return FALSE;
    }

    lpRemoteBuffer = VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (lpRemoteBuffer == NULL) {
        wprintf(L"Failed to allocate memory in remote process. Error code: %lu\n", GetLastError());
        CloseHandle(hProcess);
        return FALSE;
    }

    if (!WriteProcessMemory(hProcess, lpRemoteBuffer, lpwzDllPath, dwSize, NULL)) {
        wprintf(L"Failed to write to remote process memory. Error code: %lu\n", GetLastError());
        VirtualFreeEx(hProcess, lpRemoteBuffer, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }

    hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryW, lpRemoteBuffer, 0, &dwThreadId);
    if (hThread == NULL) {
        wprintf(L"Failed to create remote thread. Error code: %lu\n", GetLastError());
        VirtualFreeEx(hProcess, lpRemoteBuffer, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }

    WaitForSingleObject(hThread, INFINITE);

    CloseHandle(hThread);
    VirtualFreeEx(hProcess, lpRemoteBuffer, 0, MEM_RELEASE);
    CloseHandle(hProcess);

    return TRUE;
}
