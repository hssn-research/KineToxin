// Test cases for logging system
#include "../src/utils/logger.h"
#include <filesystem>

bool test_logger_initialization() {
    // Test logger initialization
    TEST_ASSERT(KineToxin::Utils::Logger::Initialize("test_logs/test.log", FALSE) == TRUE);
    
    // Cleanup
    KineToxin::Utils::Logger::Cleanup();
    
    return true;
}

bool test_logger_basic_logging() {
    TEST_ASSERT(KineToxin::Utils::Logger::Initialize("test_logs/basic.log", FALSE) == TRUE);
    
    // Test basic logging functions
    KineToxin::Utils::Logger::Info("Test info message", "TestModule");
    KineToxin::Utils::Logger::Warning("Test warning message", "TestModule");
    KineToxin::Utils::Logger::Error("Test error message", "TestModule");
    
    // Flush logs
    KineToxin::Utils::Logger::Flush();
    
    // Check if log file exists
    TEST_ASSERT(std::filesystem::exists("test_logs/basic.log"));
    
    KineToxin::Utils::Logger::Cleanup();
    return true;
}

bool test_logger_log_levels() {
    TEST_ASSERT(KineToxin::Utils::Logger::Initialize("test_logs/levels.log", FALSE) == TRUE);
    
    // Set log level to WARNING
    KineToxin::Utils::Logger::SetLogLevel(KineToxin::Utils::LogLevel::WARNING);
    
    // These should be logged
    KineToxin::Utils::Logger::Warning("This warning should appear", "TestModule");
    KineToxin::Utils::Logger::Error("This error should appear", "TestModule");
    KineToxin::Utils::Logger::Critical("This critical should appear", "TestModule");
    
    // This should not be logged (below minimum level)
    KineToxin::Utils::Logger::Info("This info should NOT appear", "TestModule");
    KineToxin::Utils::Logger::Debug("This debug should NOT appear", "TestModule");
    
    KineToxin::Utils::Logger::Flush();
    
    KineToxin::Utils::Logger::Cleanup();
    return true;
}

bool test_logger_file_output() {
    const std::string logFile = "test_logs/file_output.log";
    
    // Remove existing log file
    if (std::filesystem::exists(logFile)) {
        std::filesystem::remove(logFile);
    }
    
    TEST_ASSERT(KineToxin::Utils::Logger::Initialize(logFile, FALSE) == TRUE);
    
    KineToxin::Utils::Logger::Info("Test file output", "TestModule");
    KineToxin::Utils::Logger::Flush();
    
    // Check if file was created and has content
    TEST_ASSERT(std::filesystem::exists(logFile));
    TEST_ASSERT(std::filesystem::file_size(logFile) > 0);
    
    KineToxin::Utils::Logger::Cleanup();
    
    // Clean up test file
    if (std::filesystem::exists(logFile)) {
        std::filesystem::remove(logFile);
    }
    
    return true;
}
