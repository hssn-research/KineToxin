// Test cases for error handling system
#include "../src/utils/error.h"

bool test_error_initialization() {
    // Test error handler initialization
    TEST_ASSERT(KineToxin::Utils::ErrorHandler::Initialize() == TRUE);
    
    // Cleanup
    KineToxin::Utils::ErrorHandler::Cleanup();
    
    return true;
}

bool test_error_reporting() {
    TEST_ASSERT(KineToxin::Utils::ErrorHandler::Initialize() == TRUE);
    
    // Test error reporting
    KineToxin::Utils::ErrorHandler::ReportError(
        100, 
        KineToxin::Utils::ErrorSeverity::WARNING,
        KineToxin::Utils::ErrorCategory::GENERAL,
        "Test warning message",
        "TestModule",
        "test_function",
        42
    );
    
    // Check if error was recorded
    const KineToxin::Utils::ErrorInfo* lastError = KineToxin::Utils::ErrorHandler::GetLastError();
    TEST_ASSERT(lastError != nullptr);
    TEST_ASSERT_EQ(100, lastError->errorCode);
    TEST_ASSERT(lastError->severity == KineToxin::Utils::ErrorSeverity::WARNING);
    TEST_ASSERT(lastError->category == KineToxin::Utils::ErrorCategory::GENERAL);
    TEST_ASSERT_EQ(std::string("Test warning message"), lastError->message);
    
    KineToxin::Utils::ErrorHandler::Cleanup();
    return true;
}

bool test_error_severity_filtering() {
    TEST_ASSERT(KineToxin::Utils::ErrorHandler::Initialize() == TRUE);
    
    // Set minimum severity to ERROR
    KineToxin::Utils::ErrorHandler::SetMinimumSeverity(KineToxin::Utils::ErrorSeverity::ERROR);
    
    // Clear any existing errors
    KineToxin::Utils::ErrorHandler::ClearErrorHistory();
    
    // Report warnings (should be filtered out)
    KineToxin::Utils::ErrorHandler::ReportError(
        101, KineToxin::Utils::ErrorSeverity::WARNING,
        KineToxin::Utils::ErrorCategory::GENERAL, "Warning message"
    );
    
    // Report error (should be recorded)
    KineToxin::Utils::ErrorHandler::ReportError(
        102, KineToxin::Utils::ErrorSeverity::ERROR,
        KineToxin::Utils::ErrorCategory::GENERAL, "Error message"
    );
    
    // Check that only the error was recorded
    std::vector<KineToxin::Utils::ErrorInfo> history = KineToxin::Utils::ErrorHandler::GetErrorHistory(10);
    TEST_ASSERT_EQ(1, static_cast<int>(history.size()));
    TEST_ASSERT_EQ(102, history[0].errorCode);
    
    KineToxin::Utils::ErrorHandler::Cleanup();
    return true;
}

bool test_error_history() {
    TEST_ASSERT(KineToxin::Utils::ErrorHandler::Initialize() == TRUE);
    
    // Clear existing errors
    KineToxin::Utils::ErrorHandler::ClearErrorHistory();
    
    // Report multiple errors
    for (int i = 1; i <= 5; i++) {
        KineToxin::Utils::ErrorHandler::ReportError(
            i, KineToxin::Utils::ErrorSeverity::ERROR,
            KineToxin::Utils::ErrorCategory::GENERAL,
            "Error " + std::to_string(i)
        );
    }
    
    // Check error history
    std::vector<KineToxin::Utils::ErrorInfo> history = KineToxin::Utils::ErrorHandler::GetErrorHistory(10);
    TEST_ASSERT_EQ(5, static_cast<int>(history.size()));
    
    // Verify order (should be chronological)
    for (int i = 0; i < 5; i++) {
        TEST_ASSERT_EQ(i + 1, static_cast<int>(history[i].errorCode));
    }
    
    // Test limited history retrieval
    std::vector<KineToxin::Utils::ErrorInfo> limitedHistory = KineToxin::Utils::ErrorHandler::GetErrorHistory(3);
    TEST_ASSERT_EQ(3, static_cast<int>(limitedHistory.size()));
    
    // Should get the last 3 errors
    TEST_ASSERT_EQ(3, static_cast<int>(limitedHistory[0].errorCode));
    TEST_ASSERT_EQ(4, static_cast<int>(limitedHistory[1].errorCode));
    TEST_ASSERT_EQ(5, static_cast<int>(limitedHistory[2].errorCode));
    
    KineToxin::Utils::ErrorHandler::Cleanup();
    return true;
}
