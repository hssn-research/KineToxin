#ifndef KINETOXIN_ERROR_H
#define KINETOXIN_ERROR_H

#include <windows.h>
#include <string>
#include <vector>
#include <functional>

namespace KineToxin {
namespace Utils {

/**
 * @brief Error severity levels
 */
enum class ErrorSeverity {
    INFO = 0,
    WARNING = 1,
    ERROR = 2,
    CRITICAL = 3,
    FATAL = 4
};

/**
 * @brief Error categories for classification
 */
enum class ErrorCategory {
    GENERAL = 0,
    CRYPTO = 1,
    NETWORK = 2,
    FILE_IO = 3,
    REGISTRY = 4,
    PROCESS = 5,
    MEMORY = 6,
    EVASION = 7,
    SURVEILLANCE = 8,
    PERSISTENCE = 9
};

/**
 * @brief Error information structure
 */
struct ErrorInfo {
    DWORD errorCode;
    ErrorSeverity severity;
    ErrorCategory category;
    std::string message;
    std::string module;
    std::string function;
    DWORD lineNumber;
    SYSTEMTIME timestamp;
    
    ErrorInfo(DWORD code, ErrorSeverity sev, ErrorCategory cat, 
              const std::string& msg, const std::string& mod = "",
              const std::string& func = "", DWORD line = 0)
        : errorCode(code), severity(sev), category(cat), message(msg),
          module(mod), function(func), lineNumber(line) {
        GetSystemTime(&timestamp);
    }
};

/**
 * @brief Error handling and reporting system
 * 
 * This class provides centralized error handling with logging,
 * callback notifications, and recovery mechanisms.
 */
class ErrorHandler {
public:
    /**
     * @brief Error callback function type
     */
    using ErrorCallback = std::function<void(const ErrorInfo&)>;
    
    /**
     * @brief Initialize error handling system
     * @return TRUE if initialization successful, FALSE otherwise
     */
    static BOOL Initialize();
    
    /**
     * @brief Cleanup error handling resources
     */
    static void Cleanup();
    
    /**
     * @brief Report an error
     * @param errorCode Error code (can be Windows error code or custom)
     * @param severity Error severity
     * @param category Error category
     * @param message Error message
     * @param module Module name where error occurred
     * @param function Function name where error occurred
     * @param lineNumber Line number where error occurred
     */
    static void ReportError(DWORD errorCode, ErrorSeverity severity, 
                           ErrorCategory category, const std::string& message,
                           const std::string& module = "", 
                           const std::string& function = "",
                           DWORD lineNumber = 0);
    
    /**
     * @brief Report a Windows API error
     * @param apiFunction Name of the Windows API function that failed
     * @param module Module name where error occurred
     * @param function Function name where error occurred
     * @param lineNumber Line number where error occurred
     */
    static void ReportWinAPIError(const std::string& apiFunction,
                                 const std::string& module = "",
                                 const std::string& function = "",
                                 DWORD lineNumber = 0);
    
    /**
     * @brief Get last error information
     * @return Last error info, or nullptr if no errors
     */
    static const ErrorInfo* GetLastError();
    
    /**
     * @brief Get error history
     * @param maxCount Maximum number of errors to return
     * @return Vector of recent errors
     */
    static std::vector<ErrorInfo> GetErrorHistory(DWORD maxCount = 10);
    
    /**
     * @brief Clear error history
     */
    static void ClearErrorHistory();
    
    /**
     * @brief Set error callback for notifications
     * @param callback Callback function to call on errors
     */
    static void SetErrorCallback(ErrorCallback callback);
    
    /**
     * @brief Set minimum severity level for reporting
     * @param severity Minimum severity to report
     */
    static void SetMinimumSeverity(ErrorSeverity severity);
    
    /**
     * @brief Enable or disable error logging
     * @param enabled TRUE to enable logging, FALSE to disable
     */
    static void SetLoggingEnabled(BOOL enabled);
    
    /**
     * @brief Check if system is in critical error state
     * @return TRUE if critical errors present, FALSE otherwise
     */
    static BOOL IsInCriticalState();
    
    /**
     * @brief Format error message with details
     * @param error Error information
     * @return Formatted error message
     */
    static std::string FormatErrorMessage(const ErrorInfo& error);
    
    /**
     * @brief Convert Windows error code to string
     * @param errorCode Windows error code
     * @return Error description string
     */
    static std::string GetWindowsErrorString(DWORD errorCode);
    
    /**
     * @brief Convert error severity to string
     * @param severity Error severity
     * @return Severity string
     */
    static std::string SeverityToString(ErrorSeverity severity);
    
    /**
     * @brief Convert error category to string
     * @param category Error category
     * @return Category string
     */
    static std::string CategoryToString(ErrorCategory category);

private:
    /**
     * @brief Process and handle error
     * @param error Error information
     */
    static void ProcessError(const ErrorInfo& error);
    
    /**
     * @brief Check if error should be reported based on severity filter
     * @param severity Error severity
     * @return TRUE if should be reported, FALSE otherwise
     */
    static BOOL ShouldReportError(ErrorSeverity severity);
    
    static BOOL bInitialized;
    static BOOL bLoggingEnabled;
    static ErrorSeverity minSeverity;
    static std::vector<ErrorInfo> errorHistory;
    static ErrorCallback errorCallback;
    static CRITICAL_SECTION errorCS;
    static const DWORD MAX_ERROR_HISTORY = 100;
};

// Convenience macros for error reporting
#define REPORT_ERROR(code, severity, category, message) \
    KineToxin::Utils::ErrorHandler::ReportError(code, severity, category, message, \
                                               __FILE__, __FUNCTION__, __LINE__)

#define REPORT_WIN_ERROR(apiFunction) \
    KineToxin::Utils::ErrorHandler::ReportWinAPIError(apiFunction, \
                                                     __FILE__, __FUNCTION__, __LINE__)

#define REPORT_INFO(message) \
    REPORT_ERROR(0, KineToxin::Utils::ErrorSeverity::INFO, \
                 KineToxin::Utils::ErrorCategory::GENERAL, message)

#define REPORT_WARNING(message) \
    REPORT_ERROR(0, KineToxin::Utils::ErrorSeverity::WARNING, \
                 KineToxin::Utils::ErrorCategory::GENERAL, message)

#define REPORT_CRITICAL(message) \
    REPORT_ERROR(0, KineToxin::Utils::ErrorSeverity::CRITICAL, \
                 KineToxin::Utils::ErrorCategory::GENERAL, message)

} // namespace Utils
} // namespace KineToxin

#endif // KINETOXIN_ERROR_H
