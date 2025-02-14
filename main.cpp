#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <TlHelp32.h>  // For process and module enumeration

// Function to get the Process ID by process name
DWORD GetProcessID(const std::string& processName) {
    PROCESSENTRY32 pe32;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hSnapshot, &pe32)) {
        do {
            if (processName == pe32.szExeFile) {
                CloseHandle(hSnapshot);
                return pe32.th32ProcessID;
            }
        } while (Process32Next(hSnapshot, &pe32));
    }
    CloseHandle(hSnapshot);
    return 0;
}

// Function to get the base address of a module
uintptr_t GetModuleBaseAddress(DWORD pid, const std::string& moduleName) {
    MODULEENTRY32 modEntry;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    modEntry.dwSize = sizeof(MODULEENTRY32);
    if (Module32First(hSnapshot, &modEntry)) {
        do {
            if (moduleName == modEntry.szModule) {
                CloseHandle(hSnapshot);
                return reinterpret_cast<uintptr_t>(modEntry.modBaseAddr);
            }
        } while (Module32Next(hSnapshot, &modEntry));
    }

    CloseHandle(hSnapshot);
    return 0;
}

// Function to read memory from the target process
template <typename T>
T ReadMemory(HANDLE hProcess, uintptr_t address) {
    T buffer;
    SIZE_T bytesRead;
    if (ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(address), &buffer, sizeof(T), &bytesRead)) {
        return buffer;
    }
    else {
        std::cerr << "Failed to read memory at address: " << std::hex << address << std::endl;
        return T{};
    }
}

std::string GetServerIP(DWORD pid) {
    // Open the process
    HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (hProcess == NULL) {
        std::cerr << "Failed to open process!" << std::endl;
        return "Failed to open process.";
    }

    // Get the base address of net.dll
    uintptr_t netBase = GetModuleBaseAddress(pid, "net.dll");
    if (netBase == 0) {
        std::cerr << "Failed to find base address of net.dll!" << std::endl;
        CloseHandle(hProcess);
        return "Failed to find net.dll.";
    }

    // Calculate the address where the IP is located (netBase + 0xCD910)
    uintptr_t ipAddress = netBase + 0xCD910;

    // Read the IP address from memory
    char ip[16] = { 0 };  // Assuming it's a null-terminated string (adjust size if needed)

    SIZE_T bytesRead;
    if (ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(ipAddress), ip, sizeof(ip), &bytesRead)) {
        // Ensure we have a valid null-terminated string
        ip[sizeof(ip) - 1] = '\0';  // Force null termination just in case

        // Close the process handle
        CloseHandle(hProcess);

        return std::string(ip);
    }
    else {
        std::cerr << "Failed to read IP address from memory at: " << std::hex << ipAddress << std::endl;
        CloseHandle(hProcess);
        return "Failed to read IP address.";
    }
}


std::string GetServerPort(DWORD pid) {
    // Open the process
    HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (hProcess == NULL) {
        std::cerr << "Failed to open process!" << std::endl;
        return "Failed to open process.";
    }

    // Get the base address of net.dll
    uintptr_t netBase = GetModuleBaseAddress(pid, "nui-gsclient.dll");
    if (netBase == 0) {
        std::cerr << "Failed to find base address of nui-gsclient.dll!" << std::endl;
        CloseHandle(hProcess);
        return "Failed to find nui-gsclient.dll.";
    }

    // Calculate the address where the port is located (nui-gsclient + 0x4D032)
    uintptr_t PortAddress = netBase + 0x4D032;

    // Read the IP address from memory
    char port[6] = { 0 };  // Assuming it's a null-terminated string (adjust size if needed)

    SIZE_T bytesRead;
    if (ReadProcessMemory(hProcess, reinterpret_cast<LPCVOID>(PortAddress), port, sizeof(port), &bytesRead)) {
        // Ensure we have a valid null-terminated string
        port[sizeof(port) - 1] = '\0';  // Force null termination just in case

        // Close the process handle
        CloseHandle(hProcess);

        return std::string(port);
    }
    else {
        std::cerr << "Failed to read Port address from memory at: " << std::hex << PortAddress << std::endl;
        CloseHandle(hProcess);
        return "Failed to read Port address.";
    }
}

int main() {
    SetConsoleTitle("Server Info Grabber");
    std::vector<std::string> processes = {
        "FiveM_b2699_GTAProcess.exe",
        "FiveM_b2802_GTAProcess.exe",
        "FiveM_b2944_GTAProcess.exe",
        "FiveM_b3095_GTAProcess.exe",
        "FiveM_b3258_GTAProcess.exe"
    };

    bool serverFound = false;

    // Loop through each process name and try to get the server IP
    for (const auto& processName : processes) {
        DWORD pid = GetProcessID(processName);
        if (pid != 0) {
            std::cout << "[DEBUG] Found process: " << processName << " with PID: " << pid << std::endl;

            // Attempt to get the server IP
            std::string serverIP = GetServerIP(pid);
            std::string serverPort = GetServerPort(pid);
            if (!serverIP.empty() && serverIP != "Failed to find net.dll." && serverIP != "Failed to open process.") {
                std::cout << "Server IP: " << serverIP << ":" << serverPort << std::endl;
                serverFound = true;
                Sleep(10000000);
                break; // Stop once the server is found
            }
        }
    }
    //made by wifebeater AKA Crenix.

    //Discord is wifebeater2485
    if (!serverFound) {
        std::cout << "Join a server first." << std::endl;
    }

    return 0;
}
