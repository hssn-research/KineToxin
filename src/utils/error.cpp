#include "error.h"
#include "logger.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace KineToxin {
namespace Utils {

// Static member initialization
BOOL ErrorHandler::bInitialized = FALSE;
BOOL ErrorHandler::bLoggingEnabled = TRUE;
ErrorSeverity ErrorHandler::minSeverity = ErrorSeverity::INFO;
std::vector<ErrorInfo> ErrorHandler::errorHistory;
ErrorHandler::ErrorCallback ErrorHandler::errorCallback;
CRITICAL_SECTION ErrorHandler::errorCS;

BOOL ErrorHandler::Initialize() {
    if (bInitialized) {
        return TRUE;
    }
    
    InitializeCriticalSection(&errorCS);
    
    errorHistory.clear();
    errorHistory.reserve(MAX_ERROR_HISTORY);
    
    bInitialized = TRUE;
    
    return TRUE;
}

void ErrorHandler::Cleanup() {
    if (!bInitialized) {
        return;
    }
    
    EnterCriticalSection(&errorCS);
    
    errorHistory.clear();
    errorCallback = nullptr;
    
    LeaveCriticalSection(&errorCS);
    DeleteCriticalSection(&errorCS);
    
    bInitialized = FALSE;
}

void ErrorHandler::ReportError(DWORD errorCode, ErrorSeverity severity, 
                              ErrorCategory category, const std::string& message,
                              const std::string& module, const std::string& function,
                              DWORD lineNumber) {
    if (!bInitialized || !ShouldReportError(severity)) {
        return;
    }
    
    ErrorInfo error(errorCode, severity, category, message, 
                   module, function, lineNumber);
    
    ProcessError(error);
}

void ErrorHandler::ReportWinAPIError(const std::string& apiFunction,
                                    const std::string& module,
                                    const std::string& function,
                                    DWORD lineNumber) {
    DWORD errorCode = GetLastError();
    std::string errorMessage = apiFunction + " failed: " + GetWindowsErrorString(errorCode);
    
    ReportError(errorCode, ErrorSeverity::ERROR, ErrorCategory::GENERAL,
               errorMessage, module, function, lineNumber);
}

const ErrorInfo* ErrorHandler::GetLastError() {
    if (!bInitialized) {
        return nullptr;
    }
    
    EnterCriticalSection(&errorCS);
    
    const ErrorInfo* lastError = nullptr;
    if (!errorHistory.empty()) {
        lastError = &errorHistory.back();
    }
    
    LeaveCriticalSection(&errorCS);
    
    return lastError;
}

std::vector<ErrorInfo> ErrorHandler::GetErrorHistory(DWORD maxCount) {
    std::vector<ErrorInfo> history;
    
    if (!bInitialized) {
        return history;
    }
    
    EnterCriticalSection(&errorCS);
    
    DWORD count = min(maxCount, static_cast<DWORD>(errorHistory.size()));
    if (count > 0) {
        auto start = errorHistory.end() - count;
        history.assign(start, errorHistory.end());
    }
    
    LeaveCriticalSection(&errorCS);
    
    return history;
}

void ErrorHandler::ClearErrorHistory() {
    if (!bInitialized) {
        return;
    }
    
    EnterCriticalSection(&errorCS);
    errorHistory.clear();
    LeaveCriticalSection(&errorCS);
}

void ErrorHandler::SetErrorCallback(ErrorCallback callback) {
    if (!bInitialized) {
        return;
    }
    
    EnterCriticalSection(&errorCS);
    errorCallback = callback;
    LeaveCriticalSection(&errorCS);
}

void ErrorHandler::SetMinimumSeverity(ErrorSeverity severity) {
    minSeverity = severity;
}

void ErrorHandler::SetLoggingEnabled(BOOL enabled) {
    bLoggingEnabled = enabled;
}

BOOL ErrorHandler::IsInCriticalState() {
    if (!bInitialized) {
        return FALSE;
    }
    
    EnterCriticalSection(&errorCS);
    
    // Check for recent critical or fatal errors
    BOOL criticalState = FALSE;
    SYSTEMTIME currentTime;
    GetSystemTime(&currentTime);
    
    for (const auto& error : errorHistory) {
        if (error.severity >= ErrorSeverity::CRITICAL) {
            // Check if error occurred within last 5 minutes
            FILETIME errorTime, currentFileTime;
            SystemTimeToFileTime(&error.timestamp, &errorTime);
            SystemTimeToFileTime(&currentTime, &currentFileTime);
            
            ULARGE_INTEGER errorTime64, currentTime64;
            errorTime64.LowPart = errorTime.dwLowDateTime;
            errorTime64.HighPart = errorTime.dwHighDateTime;
            currentTime64.LowPart = currentFileTime.dwLowDateTime;
            currentTime64.HighPart = currentFileTime.dwHighDateTime;
            
            // 5 minutes in 100-nanosecond intervals
            const ULONGLONG FIVE_MINUTES = 5 * 60 * 10000000ULL;
            
            if ((currentTime64.QuadPart - errorTime64.QuadPart) < FIVE_MINUTES) {
                criticalState = TRUE;
                break;
            }
        }
    }
    
    LeaveCriticalSection(&errorCS);
    
    return criticalState;
}

std::string ErrorHandler::FormatErrorMessage(const ErrorInfo& error) {
    std::ostringstream oss;
    
    oss << "[" << SeverityToString(error.severity) << "] "
        << "[" << CategoryToString(error.category) << "] ";
    
    if (!error.module.empty()) {
        oss << "[" << error.module << "] ";
    }
    
    oss << error.message;
    
    if (error.errorCode != 0) {
        oss << " (Error Code: " << error.errorCode << ")";
    }
    
    if (!error.function.empty()) {
        oss << " in " << error.function;
        if (error.lineNumber != 0) {
            oss << ":" << error.lineNumber;
        }
    }
    
    // Add timestamp
    oss << " at " << std::setfill('0')
        << std::setw(4) << error.timestamp.wYear << "-"
        << std::setw(2) << error.timestamp.wMonth << "-"
        << std::setw(2) << error.timestamp.wDay << " "
        << std::setw(2) << error.timestamp.wHour << ":"
        << std::setw(2) << error.timestamp.wMinute << ":"
        << std::setw(2) << error.timestamp.wSecond;
    
    return oss.str();
}

std::string ErrorHandler::GetWindowsErrorString(DWORD errorCode) {
    LPSTR messageBuffer = nullptr;
    
    DWORD size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&messageBuffer,
        0,
        NULL
    );
    
    std::string message;
    if (size > 0 && messageBuffer) {
        message = std::string(messageBuffer, size);
        
        // Remove trailing newlines
        while (!message.empty() && (message.back() == '\n' || message.back() == '\r')) {
            message.pop_back();
        }
    } else {
        message = "Unknown error (0x" + std::to_string(errorCode) + ")";
    }
    
    if (messageBuffer) {
        LocalFree(messageBuffer);
    }
    
    return message;
}

std::string ErrorHandler::SeverityToString(ErrorSeverity severity) {
    switch (severity) {
        case ErrorSeverity::INFO:     return "INFO";
        case ErrorSeverity::WARNING:  return "WARNING";
        case ErrorSeverity::ERROR:    return "ERROR";
        case ErrorSeverity::CRITICAL: return "CRITICAL";
        case ErrorSeverity::FATAL:    return "FATAL";
        default:                      return "UNKNOWN";
    }
}

std::string ErrorHandler::CategoryToString(ErrorCategory category) {
    switch (category) {
        case ErrorCategory::GENERAL:     return "GENERAL";
        case ErrorCategory::CRYPTO:      return "CRYPTO";
        case ErrorCategory::NETWORK:     return "NETWORK";
        case ErrorCategory::FILE_IO:     return "FILE_IO";
        case ErrorCategory::REGISTRY:    return "REGISTRY";
        case ErrorCategory::PROCESS:     return "PROCESS";
        case ErrorCategory::MEMORY:      return "MEMORY";
        case ErrorCategory::EVASION:     return "EVASION";
        case ErrorCategory::SURVEILLANCE: return "SURVEILLANCE";
        case ErrorCategory::PERSISTENCE: return "PERSISTENCE";
        default:                         return "UNKNOWN";
    }
}

void ErrorHandler::ProcessError(const ErrorInfo& error) {
    EnterCriticalSection(&errorCS);
    
    // Add to error history
    errorHistory.push_back(error);
    
    // Limit history size
    if (errorHistory.size() > MAX_ERROR_HISTORY) {
        errorHistory.erase(errorHistory.begin());
    }
    
    // Log error if logging is enabled
    if (bLoggingEnabled) {
        std::string formattedMessage = FormatErrorMessage(error);
        
        // Map error severity to log level
        switch (error.severity) {
            case ErrorSeverity::INFO:
                Logger::Info(formattedMessage, "ErrorHandler");
                break;
            case ErrorSeverity::WARNING:
                Logger::Warning(formattedMessage, "ErrorHandler");
                break;
            case ErrorSeverity::ERROR:
                Logger::Error(formattedMessage, "ErrorHandler");
                break;
            case ErrorSeverity::CRITICAL:
            case ErrorSeverity::FATAL:
                Logger::Critical(formattedMessage, "ErrorHandler");
                break;
        }
    }
    
    // Call error callback if set
    if (errorCallback) {
        try {
            errorCallback(error);
        } catch (...) {
            // Ignore callback exceptions to prevent infinite error loops
        }
    }
    
    LeaveCriticalSection(&errorCS);
}

BOOL ErrorHandler::ShouldReportError(ErrorSeverity severity) {
    return severity >= minSeverity;
}

} // namespace Utils
} // namespace KineToxin
