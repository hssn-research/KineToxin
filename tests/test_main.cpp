#include <iostream>
#include <cassert>
#include <vector>
#include <string>

// Test framework macros
#define TEST_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            std::cerr << "ASSERTION FAILED: " << #condition << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

#define TEST_ASSERT_EQ(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            std::cerr << "ASSERTION FAILED: Expected " << (expected) << " but got " << (actual) \
                      << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            return false; \
        } \
    } while(0)

#define RUN_TEST(test_func) \
    do { \
        std::cout << "Running " << #test_func << "... "; \
        if (test_func()) { \
            std::cout << "PASSED" << std::endl; \
            passed_tests++; \
        } else { \
            std::cout << "FAILED" << std::endl; \
            failed_tests++; \
        } \
        total_tests++; \
    } while(0)

// Global test counters
static int total_tests = 0;
static int passed_tests = 0;
static int failed_tests = 0;

// Include test modules
#include "test_crypto.cpp"
#include "test_logger.cpp"
#include "test_config.cpp"
#include "test_error.cpp"
#include "test_vm_detection.cpp"

/**
 * @brief Main test runner
 */
int main() {
    std::cout << "=== KineToxin Unit Tests ===" << std::endl;
    std::cout << "Educational Malware Research Project" << std::endl;
    std::cout << "Author: Hassan Odowa" << std::endl << std::endl;
    
    // Initialize test environment
    std::cout << "Initializing test environment..." << std::endl;
    
    // Run crypto tests
    std::cout << "\n--- Cryptographic Tests ---" << std::endl;
    RUN_TEST(test_crypto_initialization);
    RUN_TEST(test_crypto_random_generation);
    RUN_TEST(test_crypto_aes_encryption);
    RUN_TEST(test_crypto_sha256_hashing);
    RUN_TEST(test_crypto_xor_operations);
    
    // Run logger tests
    std::cout << "\n--- Logger Tests ---" << std::endl;
    RUN_TEST(test_logger_initialization);
    RUN_TEST(test_logger_basic_logging);
    RUN_TEST(test_logger_log_levels);
    RUN_TEST(test_logger_file_output);
    
    // Run config tests
    std::cout << "\n--- Configuration Tests ---" << std::endl;
    RUN_TEST(test_config_initialization);
    RUN_TEST(test_config_string_values);
    RUN_TEST(test_config_numeric_values);
    RUN_TEST(test_config_boolean_values);
    RUN_TEST(test_config_persistence);
    
    // Run error handling tests
    std::cout << "\n--- Error Handling Tests ---" << std::endl;
    RUN_TEST(test_error_initialization);
    RUN_TEST(test_error_reporting);
    RUN_TEST(test_error_severity_filtering);
    RUN_TEST(test_error_history);
    
    // Run VM detection tests
    std::cout << "\n--- VM Detection Tests ---" << std::endl;
    RUN_TEST(test_vm_detection_basic);
    RUN_TEST(test_vm_detection_processes);
    RUN_TEST(test_vm_detection_registry);
    RUN_TEST(test_vm_detection_timing);
    
    // Print test summary
    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "Total tests: " << total_tests << std::endl;
    std::cout << "Passed: " << passed_tests << std::endl;
    std::cout << "Failed: " << failed_tests << std::endl;
    std::cout << "Success rate: " << (total_tests > 0 ? (passed_tests * 100 / total_tests) : 0) << "%" << std::endl;
    
    return (failed_tests == 0) ? 0 : 1;
}
