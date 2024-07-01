#ifndef PID_H
#define PID_H
#include <windows.h>
#if !defined(__AW_SUFFIXED__)
#if defined(UNICODE) || defined(_UNICODE)
#define __AW_SUFFIXED__(__NAME__) __NAME__##W
#else
#define __AW_SUFFIXED__(__NAME__) __NAME__##A
#endif // UNICODE or _UNICODE
#endif // __AW_SUFFIXED__
// Function to get PID by process name
#define GetPIDByProcessName(tstrProcessName) __AW_SUFFIXED__(GetPIDByProcessName)(tstrProcessName)
/**
 * @brief Retrieves the process ID (PID) by its ASCII name.
 * @param lpszProcessName Pointer to the null-terminated ASCII string containing the process name.
 * @return The process ID (PID) if successful, or zero if the process name was not found.
 */
DWORD GetPIDByProcessNameA(LPCSTR lpszProcessName);
/**
 * @brief Retrieves the process ID (PID) by its Unicode name.
 * @param lpwzProcessName Pointer to the null-terminated Unicode string containing the process name.
 * @return The process ID (PID) if successful, or zero if the process name was not found.
 */
DWORD GetPIDByProcessNameW(LPCWSTR lpwzProcessName);
// Function to get PID by window title
#define GetPIDByWindowTitle(tstrWindowTitle) __AW_SUFFIXED__(GetPIDByWindowTitle)(tstrWindowTitle)
/**
 * @brief Retrieves the process ID (PID) by the window title.
 * @param lpszWindowTitle Pointer to the null-terminated ASCII string containing the window title.
 * @return The process ID (PID) if successful, or zero if the window title was not found.
 */
DWORD GetPIDByWindowTitleA(LPCSTR lpszWindowTitle);
/**
 * @brief Retrieves the process ID (PID) by the window title.
 * @param lpwzWindowTitle Pointer to the null-terminated Unicode string containing the window title.
 * @return The process ID (PID) if successful, or zero if the window title was not found.
 */
DWORD GetPIDByWindowTitleW(LPCWSTR lpwzWindowTitle);
#endif // PID_H