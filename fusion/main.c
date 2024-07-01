#include "pid.h"
#include "inject.h"
#include <windows.h>
#include <commdlg.h>
#include <wchar.h>
#include <tchar.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
/**
 * @brief Checks if the current process is running with administrator privileges.
 *
 * @return TRUE if the current process has administrator privileges, FALSE otherwise.
 */
BOOL IsAdmin(VOID) {
    BOOL isAdmin = FALSE;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    PSID AdministratorsGroup;
    // Create a SID for the administrators group
    if (AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &AdministratorsGroup)) {
        // Check if the current token is a member of the administrators group
        if (!CheckTokenMembership(NULL, AdministratorsGroup, &isAdmin)) {
            isAdmin = FALSE; // Failed to determine membership
        }
        FreeSid(AdministratorsGroup);
    }
    return isAdmin;
}
// Polyfills for string copy functions depending on the C standard
#if __STDC_VERSION__ <= 201112L
/**
 * @brief Copies a string (ANSI version).
 *
 * @param dest Destination buffer.
 * @param destsz Size of the destination buffer.
 * @param src Source string to copy.
 * @return Zero on success, ERANGE if truncation occurs, or EINVAL on error.
 */
int _strcpy_s(char* dest, size_t destsz, const char* src) {
    if (!dest || !src) return EINVAL;
    if (destsz == 0) return EINVAL;
    size_t i;
    for (i = 0; i < destsz; i++) {
        dest[i] = src[i];
        if (src[i] == '\0') return 0;
    }
    dest[destsz - 1] = '\0';
    return ERANGE;
}
/**
 * @brief Copies a wide string (Unicode version).
 *
 * @param dest Destination buffer.
 * @param destsz Size of the destination buffer.
 * @param src Source wide string to copy.
 * @return Zero on success, ERANGE if truncation occurs, or EINVAL on error.
 */
int _wcscpy_s(wchar_t* dest, size_t destsz, const wchar_t* src) {
    if (!dest || !src) return EINVAL;
    if (destsz == 0) return EINVAL;
    size_t i;
    for (i = 0; i < destsz; i++) {
        dest[i] = src[i];
        if (src[i] == L'\0') return 0;
    }
    dest[destsz - 1] = L'\0';
    return ERANGE;
}
#endif
// Macro definition for _tcscpy_s based on UNICODE or _UNICODE
#ifndef _tcscpy_s
#if defined(UNICODE) || defined(_UNICODE)
#define _tcscpy_s _wcscpy_s
#else
#define _tcscpy_s _strcpy_s
#endif // UNICODE or _UNICODE
#endif // _tcscpy_s
// Macro definition for _tstol based on UNICODE or _UNICODE
#ifndef _tstol
#if defined(UNICODE) || defined(_UNICODE)
#define _tstol wcstol
#else
#define _tstol strtol
#endif // UNICODE or _UNICODE
#endif // _tstol
// Enumeration for argument modes related to PID handling
typedef enum _E_PIDArgMode {
    PidMode_None, ///< No mode specified.
    PidMode_Pid,  ///< Mode for specifying a PID.
    PidMode_Wnd,  ///< Mode for specifying a window name.
    PidMode_Proc  ///< Mode for specifying a process name.
} E_PIDArgMode;
// Enumeration for argument modes related to DLL handling
typedef enum _E_DLLArgMode {
    DllMode_None,   ///< No mode specified.
    DllMode_Path,   ///< Mode for specifying a DLL file path.
    DllMode_Dialog  ///< Mode for opening a DLL selection dialog.
} E_DLLArgMode;
/**
 * @brief Opens a dialog to select a DLL file.
 *
 * @param lpDllPath Buffer to store the selected DLL file path.
 * @param nMaxPath Maximum size of lpDllPath buffer.
 * @return TRUE if a file is selected successfully, FALSE otherwise.
 */
BOOL OpenDllDialog(LPTSTR lpDllPath, DWORD nMaxPath) {
    OPENFILENAME ofn;
    TCHAR szFile[MAX_PATH] = { 0 };

    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = _T("DLL files\0*.dll\0All files\0*.*\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE) {
        _tcscpy_s(lpDllPath, nMaxPath, szFile);
        return TRUE;
    }
    else {
        return FALSE;
    }
}
/**
 * @brief Main function that handles command line arguments and loads a DLL into a process.
 *
 * @param argc Number of command line arguments.
 * @param argv Array of command line arguments.
 * @return EXIT_SUCCESS if successful, EXIT_FAILURE otherwise.
 */
INT _tmain(INT argc, LPTSTR argv[]) {
    // Check for correct command line arguments
    if (argc < 3 || (argc % 2 == 0)) {
        _tprintf(_T("Usage: %s { /pid <PID> | /wnd <WindowName> | /proc <ProcessName> } /dll <Path or dialog>\n"), argv[0]);
        return 1;
    }
    if (!IsAdmin()) {
        _tprintf(_T("Warning: This application requires administrator rights to function properly.\n"));
    }
    DWORD dwProcessId = 0;
    TCHAR szWindowTitle[MAX_PATH] = { 0 };
    TCHAR szProcessName[MAX_PATH] = { 0 };
    TCHAR szDllPath[MAX_PATH] = { 0 };
    E_PIDArgMode pidMode = PidMode_None;
    E_DLLArgMode dllMode = DllMode_None;
    // Parse command line arguments
    for (int i = 1; i < argc; i += 2) {
        // Handle /pid argument
        if (_tcsicmp(argv[i], _T("/pid")) == 0 && pidMode == PidMode_None) {
            dwProcessId = _tstol(argv[i + 1], NULL, 10);
            pidMode = PidMode_Pid;
        }
        // Handle /wnd argument
        else if (_tcsicmp(argv[i], _T("/wnd")) == 0 && pidMode == PidMode_None) {
            _tcscpy_s(szWindowTitle, MAX_PATH, argv[i + 1]);
            pidMode = PidMode_Wnd;
        }
        // Handle /proc argument
        else if (_tcsicmp(argv[i], _T("/proc")) == 0 && pidMode == PidMode_None) {
            _tcscpy_s(szProcessName, MAX_PATH, argv[i + 1]);
            pidMode = PidMode_Proc;
        }
        // Handle /dll argument
        else if (_tcsicmp(argv[i], _T("/dll")) == 0) {
            if (_tcsncmp(argv[i + 1], _T("dialog"), 6) == 0) {
                // Open DLL file dialog
                if (!OpenDllDialog(szDllPath, MAX_PATH)) {
                    _tprintf(_T("DLL selection canceled.\n"));
                    return EXIT_FAILURE;
                }
                dllMode = DllMode_Dialog;
            }
            else {
                _tcscpy_s(szDllPath, MAX_PATH, argv[i + 1]);
                dllMode = DllMode_Path;
            }
        }
    }
    // Check if at least one PID, window name, or process name is specified
    if (pidMode == PidMode_None) {
        _tprintf(_T("Error: Must specify one of /pid, /wnd, or /proc.\n"));
        return 1;
    }
    // Resolve the specified PID or process name to a process ID
    switch (pidMode) {
    case PidMode_Wnd:
        dwProcessId = GetPIDByWindowTitle(szWindowTitle);
        break;
    case PidMode_Proc:
        dwProcessId = GetPIDByProcessName(szProcessName);
        break;
    default:
        break;
    }
    // Load the selected DLL into the specified process
    if (LoadDLL(dwProcessId, szDllPath)) {
        _tprintf(_T("DLL [%s] Injected into process [id: %lu]\n"), szDllPath, dwProcessId);
    }
    else {
        // Print selected DLL path and process ID for verification
        _tprintf(_T("An error occurred during injection, google the line above.\n"));
        _tprintf(_T("Values in the environment:\n"));
        _tprintf(_T("szDllPath: %s\n"), szDllPath);
        _tprintf(_T("dwProcessId: %lu\n"), dwProcessId);
    }
    return EXIT_SUCCESS;
}