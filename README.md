# Fusion
A Windows injector written in C, using WinAPI.

## Build Instructions

1. Open the "fusion.sln" file using Visual Studio 20xx.
2. Select Release configuration (for optimized performance).
3. Choose the platform:
   - Select x86 to inject into 32-bit applications only.
   - Select x64 to inject into both 32-bit and 64-bit applications.
4. Press **Ctrl + B** to build the project.
5. The output file will be saved at:
   - x86: fusion\Release\fusion.exe
   - x64: fusion\x64\Release\fusion.exe
6. Build complete.

## Usage

- First argument: (/pid, /proc, or /wnd)
  - If using `/pid`, specify the process ID after it (obtainable from Task Manager).
  - If using `/proc`, specify the process name after it (also obtainable from Task Manager).
  - If using `/wnd`, specify the window title (often accessible by hovering over the taskbar icon).

- Second argument: (/dll)
  - After `/dll`, provide the path to the DLL to inject.
  - To open a file dialog instead, use 'dialog' after `/dll` (without specifying a path).
