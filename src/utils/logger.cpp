#include "logger.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <filesystem>

namespace KineToxin {
namespace Utils {

// Static member initialization
BOOL Logger::bInitialized = FALSE;
BOOL Logger::bEncryptLogs = TRUE;
BOOL Logger::bConsoleOutput = FALSE;
LogLevel Logger::minLogLevel = LogLevel::INFO;
std::string Logger::logFilePath;
std::ofstream Logger::logFileStream;
std::mutex Logger::logMutex;
DWORD Logger::maxLogFileSize = 10485760; // 10MB
std::vector<BYTE> Logger::encryptionKey;

BOOL Logger::Initialize(const std::string& logFile, BOOL encryptLogs, DWORD maxLogSize) {
    std::lock_guard<std::mutex> lock(logMutex);
    
    if (bInitialized) {
        return TRUE;
    }
    
    logFilePath = logFile;
    bEncryptLogs = encryptLogs;
    maxLogFileSize = maxLogSize;
    
    // Create log directory if it doesn't exist
    std::filesystem::path logPath(logFile);
    std::filesystem::path logDir = logPath.parent_path();
    
    if (!logDir.empty() && !std::filesystem::exists(logDir)) {
        std::filesystem::create_directories(logDir);
    }
    
    // Open log file
    logFileStream.open(logFilePath, std::ios::app | std::ios::binary);
    if (!logFileStream.is_open()) {
        return FALSE;
    }
    
    // Generate encryption key if encryption is enabled
    if (bEncryptLogs) {
        if (!Crypto::Initialize()) {
            logFileStream.close();
            return FALSE;
        }
        
        if (!Crypto::GenerateRandomBytes(32, encryptionKey)) {
            logFileStream.close();
            Crypto::Cleanup();
            return FALSE;
        }
    }
    
    bInitialized = TRUE;
    
    // Log initialization message
    Info("Logger initialized successfully", "Logger");
    
    return TRUE;
}

void Logger::Cleanup() {
    std::lock_guard<std::mutex> lock(logMutex);
    
    if (!bInitialized) {
        return;
    }
    
    Info("Logger shutting down", "Logger");
    Flush();
    
    if (logFileStream.is_open()) {
        logFileStream.close();
    }
    
    if (bEncryptLogs) {
        Crypto::Cleanup();
        encryptionKey.clear();
    }
    
    bInitialized = FALSE;
}

void Logger::SetLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(logMutex);
    minLogLevel = level;
}

void Logger::Log(LogLevel level, const std::string& message, const std::string& module) {
    if (!bInitialized || level < minLogLevel) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(logMutex);
    
    // Format log entry
    std::ostringstream oss;
    oss << "[" << GetTimestamp() << "] "
        << "[" << LogLevelToString(level) << "] ";
    
    if (!module.empty()) {
        oss << "[" << module << "] ";
    }
    
    oss << message << std::endl;
    
    std::string logEntry = oss.str();
    
    // Console output if enabled
    if (bConsoleOutput) {
        std::cout << logEntry;
    }
    
    // Write to file
    WriteLogEntry(logEntry);
    
    // Check if log rotation is needed
    if (logFileStream.tellp() > static_cast<std::streampos>(maxLogFileSize)) {
        RotateLogFile();
    }
}

void Logger::Debug(const std::string& message, const std::string& module) {
    Log(LogLevel::DEBUG, message, module);
}

void Logger::Info(const std::string& message, const std::string& module) {
    Log(LogLevel::INFO, message, module);
}

void Logger::Warning(const std::string& message, const std::string& module) {
    Log(LogLevel::WARNING, message, module);
}

void Logger::Error(const std::string& message, const std::string& module) {
    Log(LogLevel::ERROR, message, module);
}

void Logger::Critical(const std::string& message, const std::string& module) {
    Log(LogLevel::CRITICAL, message, module);
}

void Logger::Flush() {
    if (bInitialized && logFileStream.is_open()) {
        logFileStream.flush();
    }
}

BOOL Logger::RotateLogFile() {
    if (!bInitialized) {
        return FALSE;
    }
    
    // Close current log file
    logFileStream.close();
    
    // Rename current log file with timestamp
    std::string timestamp = GetTimestamp();
    std::replace(timestamp.begin(), timestamp.end(), ':', '-');
    std::replace(timestamp.begin(), timestamp.end(), ' ', '_');
    
    std::string rotatedLogPath = logFilePath + "." + timestamp;
    
    try {
        std::filesystem::rename(logFilePath, rotatedLogPath);
    } catch (const std::exception&) {
        // If rotation fails, continue with current file
    }
    
    // Open new log file
    logFileStream.open(logFilePath, std::ios::app | std::ios::binary);
    if (!logFileStream.is_open()) {
        return FALSE;
    }
    
    Info("Log file rotated", "Logger");
    return TRUE;
}

void Logger::SetConsoleOutput(BOOL enabled) {
    std::lock_guard<std::mutex> lock(logMutex);
    bConsoleOutput = enabled;
}

std::string Logger::GetTimestamp() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    
    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(4) << st.wYear << "-"
        << std::setw(2) << st.wMonth << "-"
        << std::setw(2) << st.wDay << " "
        << std::setw(2) << st.wHour << ":"
        << std::setw(2) << st.wMinute << ":"
        << std::setw(2) << st.wSecond << "."
        << std::setw(3) << st.wMilliseconds;
    
    return oss.str();
}

std::string Logger::LogLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:    return "DEBUG";
        case LogLevel::INFO:     return "INFO";
        case LogLevel::WARNING:  return "WARN";
        case LogLevel::ERROR:    return "ERROR";
        case LogLevel::CRITICAL: return "CRIT";
        default:                 return "UNKNOWN";
    }
}

void Logger::WriteLogEntry(const std::string& entry) {
    if (!logFileStream.is_open()) {
        return;
    }
    
    if (bEncryptLogs && !encryptionKey.empty()) {
        std::vector<BYTE> encrypted;
        if (EncryptLogEntry(entry, encrypted)) {
            // Write encrypted data with length prefix
            DWORD dataLength = static_cast<DWORD>(encrypted.size());
            logFileStream.write(reinterpret_cast<const char*>(&dataLength), sizeof(dataLength));
            logFileStream.write(reinterpret_cast<const char*>(encrypted.data()), encrypted.size());
        }
    } else {
        // Write plain text
        logFileStream << entry;
    }
    
    logFileStream.flush();
}

BOOL Logger::EncryptLogEntry(const std::string& entry, std::vector<BYTE>& encrypted) {
    if (!bEncryptLogs || encryptionKey.empty()) {
        return FALSE;
    }
    
    // Convert string to byte vector
    std::vector<BYTE> data(entry.begin(), entry.end());
    
    // Encrypt the data
    return Crypto::EncryptAES256(data, encryptionKey, encrypted);
}

} // namespace Utils
} // namespace KineToxin
