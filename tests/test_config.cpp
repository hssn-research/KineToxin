// Test cases for configuration system
#include "../src/utils/config.h"
#include <filesystem>

bool test_config_initialization() {
    // Test config system initialization
    TEST_ASSERT(KineToxin::Utils::Config::Initialize("test_config/test.conf", FALSE) == TRUE);
    
    // Cleanup
    KineToxin::Utils::Config::Cleanup();
    
    return true;
}

bool test_config_string_values() {
    TEST_ASSERT(KineToxin::Utils::Config::Initialize("test_config/strings.conf", FALSE) == TRUE);
    
    // Test setting and getting string values
    KineToxin::Utils::Config::SetString("test", "name", "KineToxin");
    KineToxin::Utils::Config::SetString("test", "version", "1.0.0");
    
    TEST_ASSERT_EQ(std::string("KineToxin"), KineToxin::Utils::Config::GetString("test", "name"));
    TEST_ASSERT_EQ(std::string("1.0.0"), KineToxin::Utils::Config::GetString("test", "version"));
    
    // Test default values
    TEST_ASSERT_EQ(std::string("default"), KineToxin::Utils::Config::GetString("test", "nonexistent", "default"));
    
    KineToxin::Utils::Config::Cleanup();
    return true;
}

bool test_config_numeric_values() {
    TEST_ASSERT(KineToxin::Utils::Config::Initialize("test_config/numbers.conf", FALSE) == TRUE);
    
    // Test integer values
    KineToxin::Utils::Config::SetInt("test", "count", 42);
    KineToxin::Utils::Config::SetInt("test", "negative", -100);
    
    TEST_ASSERT_EQ(42, KineToxin::Utils::Config::GetInt("test", "count"));
    TEST_ASSERT_EQ(-100, KineToxin::Utils::Config::GetInt("test", "negative"));
    TEST_ASSERT_EQ(999, KineToxin::Utils::Config::GetInt("test", "nonexistent", 999));
    
    // Test float values
    KineToxin::Utils::Config::SetFloat("test", "pi", 3.14159);
    KineToxin::Utils::Config::SetFloat("test", "negative_float", -2.5);
    
    TEST_ASSERT(abs(3.14159 - KineToxin::Utils::Config::GetFloat("test", "pi")) < 0.00001);
    TEST_ASSERT(abs(-2.5 - KineToxin::Utils::Config::GetFloat("test", "negative_float")) < 0.00001);
    
    KineToxin::Utils::Config::Cleanup();
    return true;
}

bool test_config_boolean_values() {
    TEST_ASSERT(KineToxin::Utils::Config::Initialize("test_config/booleans.conf", FALSE) == TRUE);
    
    // Test boolean values
    KineToxin::Utils::Config::SetBool("test", "enabled", true);
    KineToxin::Utils::Config::SetBool("test", "disabled", false);
    
    TEST_ASSERT_EQ(true, KineToxin::Utils::Config::GetBool("test", "enabled"));
    TEST_ASSERT_EQ(false, KineToxin::Utils::Config::GetBool("test", "disabled"));
    TEST_ASSERT_EQ(true, KineToxin::Utils::Config::GetBool("test", "nonexistent", true));
    
    KineToxin::Utils::Config::Cleanup();
    return true;
}

bool test_config_persistence() {
    const std::string configFile = "test_config/persistence.conf";
    
    // Remove existing config file
    if (std::filesystem::exists(configFile)) {
        std::filesystem::remove(configFile);
    }
    
    // First session - create and save config
    {
        TEST_ASSERT(KineToxin::Utils::Config::Initialize(configFile, FALSE) == TRUE);
        
        KineToxin::Utils::Config::SetString("app", "name", "KineToxin");
        KineToxin::Utils::Config::SetInt("app", "version", 100);
        KineToxin::Utils::Config::SetBool("app", "debug", true);
        
        TEST_ASSERT(KineToxin::Utils::Config::SaveConfig() == TRUE);
        KineToxin::Utils::Config::Cleanup();
    }
    
    // Second session - load and verify config
    {
        TEST_ASSERT(KineToxin::Utils::Config::Initialize(configFile, FALSE) == TRUE);
        TEST_ASSERT(KineToxin::Utils::Config::LoadConfig() == TRUE);
        
        TEST_ASSERT_EQ(std::string("KineToxin"), KineToxin::Utils::Config::GetString("app", "name"));
        TEST_ASSERT_EQ(100, KineToxin::Utils::Config::GetInt("app", "version"));
        TEST_ASSERT_EQ(true, KineToxin::Utils::Config::GetBool("app", "debug"));
        
        KineToxin::Utils::Config::Cleanup();
    }
    
    // Clean up test file
    if (std::filesystem::exists(configFile)) {
        std::filesystem::remove(configFile);
    }
    
    return true;
}
