#include <windows.h>             // For various Windows API functions like CreateToolhelp32Snapshot, CloseHandle, Process32Next, etc.
#include <psapi.h>               // For memory-related functions like ReadProcessMemory, GetModuleFileNameEx
#include <tlhelp32.h>            // For Toolhelp API (CreateToolhelp32Snapshot, Module32First, Module32Next, etc.)
#include <cstdint>               // For standard integer types like uint32_t, uint64_t
#include <memory>                // For smart pointers like std::unique_ptr
#include <string_view>           // For std::string_view to handle strings efficiently
#include <string>                // For std::string (used in get_process_name_from_hwnd)
#include <iostream>              // For std::cout and debugging output
#include <cctype>                // For std::isdigit (in extract_number)
#include <tchar.h>               // For _tcsicmp (wide char support in Windows API)

std::string GetCurrentServerEndPoint();
std::string GetPort();
