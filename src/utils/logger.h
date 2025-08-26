#ifndef KINETOXIN_LOGGER_H
#define KINETOXIN_LOGGER_H

#include <windows.h>
#include <string>
#include <fstream>
#include <mutex>
#include <vector>
#include "crypto.h"

namespace KineToxin {
namespace Utils {

/**
 * @brief Log levels for the logging system
 */
enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    CRITICAL = 4
};

/**
 * @brief Encrypted logging system for KineToxin
 * 
 * This class provides thread-safe logging with encryption capabilities
 * to prevent log analysis by security tools.
 */
class Logger {
public:
    /**
     * @brief Initialize the logging system
     * @param logFile Path to the log file
     * @param encryptLogs Whether to encrypt log entries
     * @param maxLogSize Maximum log file size in bytes
     * @return TRUE if initialization successful, FALSE otherwise
     */
    static BOOL Initialize(const std::string& logFile, 
                          BOOL encryptLogs = TRUE,
                          DWORD maxLogSize = 10485760); // 10MB default
    
    /**
     * @brief Cleanup logging resources
     */
    static void Cleanup();
    
    /**
     * @brief Set minimum log level
     * @param level Minimum level to log
     */
    static void SetLogLevel(LogLevel level);
    
    /**
     * @brief Log a message
     * @param level Log level
     * @param message Message to log
     * @param module Optional module name
     */
    static void Log(LogLevel level, const std::string& message, 
                   const std::string& module = "");
    
    /**
     * @brief Log debug message
     * @param message Message to log
     * @param module Optional module name
     */
    static void Debug(const std::string& message, const std::string& module = "");
    
    /**
     * @brief Log info message
     * @param message Message to log
     * @param module Optional module name
     */
    static void Info(const std::string& message, const std::string& module = "");
    
    /**
     * @brief Log warning message
     * @param message Message to log
     * @param module Optional module name
     */
    static void Warning(const std::string& message, const std::string& module = "");
    
    /**
     * @brief Log error message
     * @param message Message to log
     * @param module Optional module name
     */
    static void Error(const std::string& message, const std::string& module = "");
    
    /**
     * @brief Log critical message
     * @param message Message to log
     * @param module Optional module name
     */
    static void Critical(const std::string& message, const std::string& module = "");
    
    /**
     * @brief Flush log buffer to file
     */
    static void Flush();
    
    /**
     * @brief Rotate log file if it exceeds size limit
     * @return TRUE if rotation successful, FALSE otherwise
     */
    static BOOL RotateLogFile();
    
    /**
     * @brief Enable or disable console output
     * @param enabled TRUE to enable console output, FALSE to disable
     */
    static void SetConsoleOutput(BOOL enabled);

private:
    /**
     * @brief Get current timestamp string
     * @return Formatted timestamp
     */
    static std::string GetTimestamp();
    
    /**
     * @brief Convert log level to string
     * @param level Log level
     * @return String representation
     */
    static std::string LogLevelToString(LogLevel level);
    
    /**
     * @brief Write log entry to file
     * @param entry Log entry to write
     */
    static void WriteLogEntry(const std::string& entry);
    
    /**
     * @brief Encrypt log entry if encryption is enabled
     * @param entry Plain text log entry
     * @param encrypted Output encrypted entry
     * @return TRUE if encryption successful, FALSE otherwise
     */
    static BOOL EncryptLogEntry(const std::string& entry, std::vector<BYTE>& encrypted);
    
    static BOOL bInitialized;
    static BOOL bEncryptLogs;
    static BOOL bConsoleOutput;
    static LogLevel minLogLevel;
    static std::string logFilePath;
    static std::ofstream logFileStream;
    static std::mutex logMutex;
    static DWORD maxLogFileSize;
    static std::vector<BYTE> encryptionKey;
};

// Convenience macros for logging
#define LOG_DEBUG(msg, module) KineToxin::Utils::Logger::Debug(msg, module)
#define LOG_INFO(msg, module) KineToxin::Utils::Logger::Info(msg, module)
#define LOG_WARNING(msg, module) KineToxin::Utils::Logger::Warning(msg, module)
#define LOG_ERROR(msg, module) KineToxin::Utils::Logger::Error(msg, module)
#define LOG_CRITICAL(msg, module) KineToxin::Utils::Logger::Critical(msg, module)

} // namespace Utils
} // namespace KineToxin

#endif // KINETOXIN_LOGGER_H
