#ifndef INJECT_H
#define INJECT_H
#include <windows.h>
#ifndef __AW_SUFFIXED__
#if defined(UNICODE) || defined(_UNICODE)
#define __AW_SUFFIXED__(__NAME__) __NAME__##W
#else
#define __AW_SUFFIXED__(__NAME__) __NAME__##A
#endif // UNICODE or _UNICODE
#endif // __AW_SUFFIXED__
// Function to load a DLL into a process
#define LoadDLL(dwProcessId, tstrDllPath) __AW_SUFFIXED__(LoadDLL)(dwProcessId, tstrDllPath)
/**
 * @brief Loads a DLL into a specified process using its ASCII path.
 * @param dwProcessId The process ID of the target process.
 * @param lpszDllPath Pointer to a null-terminated ASCII string specifying the path to the DLL.
 * @return TRUE if the DLL was successfully injected, FALSE otherwise.
 */
BOOL LoadDLLA(DWORD dwProcessId, LPCSTR lpszDllPath);
/**
 * @brief Loads a DLL into a specified process using its Unicode path.
 * @param dwProcessId The process ID of the target process.
 * @param lpwzDllPath Pointer to a null-terminated Unicode string specifying the path to the DLL.
 * @return TRUE if the DLL was successfully injected, FALSE otherwise.
 */
BOOL LoadDLLW(DWORD dwProcessId, LPCWSTR lpwzDllPath);
#endif // INJECT_H