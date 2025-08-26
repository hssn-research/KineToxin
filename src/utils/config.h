#ifndef KINETOXIN_CONFIG_H
#define KINETOXIN_CONFIG_H

#include <windows.h>
#include <string>
#include <map>
#include <vector>
#include <mutex>

namespace KineToxin {
namespace Utils {

/**
 * @brief Configuration value types
 */
enum class ConfigValueType {
    STRING,
    INTEGER,
    BOOLEAN,
    FLOAT
};

/**
 * @brief Configuration value wrapper
 */
struct ConfigValue {
    ConfigValueType type;
    std::string stringValue;
    int intValue;
    bool boolValue;
    double floatValue;
    
    ConfigValue() : type(ConfigValueType::STRING), intValue(0), boolValue(false), floatValue(0.0) {}
    
    explicit ConfigValue(const std::string& value) 
        : type(ConfigValueType::STRING), stringValue(value), intValue(0), boolValue(false), floatValue(0.0) {}
    
    explicit ConfigValue(int value) 
        : type(ConfigValueType::INTEGER), intValue(value), boolValue(false), floatValue(0.0) {}
    
    explicit ConfigValue(bool value) 
        : type(ConfigValueType::BOOLEAN), intValue(0), boolValue(value), floatValue(0.0) {}
    
    explicit ConfigValue(double value) 
        : type(ConfigValueType::FLOAT), intValue(0), boolValue(false), floatValue(value) {}
};

/**
 * @brief Configuration management system for KineToxin
 * 
 * This class provides encrypted configuration storage and retrieval
 * with support for multiple data types and sections.
 */
class Config {
public:
    /**
     * @brief Initialize configuration system
     * @param configFile Path to configuration file
     * @param encrypted Whether to encrypt configuration data
     * @return TRUE if initialization successful, FALSE otherwise
     */
    static BOOL Initialize(const std::string& configFile, BOOL encrypted = TRUE);
    
    /**
     * @brief Cleanup configuration resources
     */
    static void Cleanup();
    
    /**
     * @brief Load configuration from file
     * @return TRUE if loading successful, FALSE otherwise
     */
    static BOOL LoadConfig();
    
    /**
     * @brief Save configuration to file
     * @return TRUE if saving successful, FALSE otherwise
     */
    static BOOL SaveConfig();
    
    /**
     * @brief Set string configuration value
     * @param section Configuration section
     * @param key Configuration key
     * @param value String value
     */
    static void SetString(const std::string& section, const std::string& key, 
                         const std::string& value);
    
    /**
     * @brief Get string configuration value
     * @param section Configuration section
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return String value
     */
    static std::string GetString(const std::string& section, const std::string& key, 
                                const std::string& defaultValue = "");
    
    /**
     * @brief Set integer configuration value
     * @param section Configuration section
     * @param key Configuration key
     * @param value Integer value
     */
    static void SetInt(const std::string& section, const std::string& key, int value);
    
    /**
     * @brief Get integer configuration value
     * @param section Configuration section
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Integer value
     */
    static int GetInt(const std::string& section, const std::string& key, 
                     int defaultValue = 0);
    
    /**
     * @brief Set boolean configuration value
     * @param section Configuration section
     * @param key Configuration key
     * @param value Boolean value
     */
    static void SetBool(const std::string& section, const std::string& key, bool value);
    
    /**
     * @brief Get boolean configuration value
     * @param section Configuration section
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Boolean value
     */
    static bool GetBool(const std::string& section, const std::string& key, 
                       bool defaultValue = false);
    
    /**
     * @brief Set float configuration value
     * @param section Configuration section
     * @param key Configuration key
     * @param value Float value
     */
    static void SetFloat(const std::string& section, const std::string& key, double value);
    
    /**
     * @brief Get float configuration value
     * @param section Configuration section
     * @param key Configuration key
     * @param defaultValue Default value if key not found
     * @return Float value
     */
    static double GetFloat(const std::string& section, const std::string& key, 
                          double defaultValue = 0.0);
    
    /**
     * @brief Check if configuration key exists
     * @param section Configuration section
     * @param key Configuration key
     * @return TRUE if key exists, FALSE otherwise
     */
    static BOOL HasKey(const std::string& section, const std::string& key);
    
    /**
     * @brief Remove configuration key
     * @param section Configuration section
     * @param key Configuration key
     * @return TRUE if key removed, FALSE if not found
     */
    static BOOL RemoveKey(const std::string& section, const std::string& key);
    
    /**
     * @brief Get all keys in a section
     * @param section Configuration section
     * @return Vector of key names
     */
    static std::vector<std::string> GetSectionKeys(const std::string& section);
    
    /**
     * @brief Get all section names
     * @return Vector of section names
     */
    static std::vector<std::string> GetSections();
    
    /**
     * @brief Set default configuration values
     */
    static void SetDefaults();

private:
    /**
     * @brief Create section key for internal storage
     * @param section Section name
     * @param key Key name
     * @return Combined section.key string
     */
    static std::string CreateSectionKey(const std::string& section, const std::string& key);
    
    /**
     * @brief Parse configuration file content
     * @param content File content to parse
     * @return TRUE if parsing successful, FALSE otherwise
     */
    static BOOL ParseConfigContent(const std::string& content);
    
    /**
     * @brief Generate configuration file content
     * @return Configuration content as string
     */
    static std::string GenerateConfigContent();
    
    /**
     * @brief Encrypt configuration data
     * @param data Plain text data
     * @param encrypted Output encrypted data
     * @return TRUE if encryption successful, FALSE otherwise
     */
    static BOOL EncryptConfigData(const std::string& data, std::vector<BYTE>& encrypted);
    
    /**
     * @brief Decrypt configuration data
     * @param encrypted Encrypted data
     * @param data Output plain text data
     * @return TRUE if decryption successful, FALSE otherwise
     */
    static BOOL DecryptConfigData(const std::vector<BYTE>& encrypted, std::string& data);
    
    /**
     * @brief Trim whitespace from string
     * @param str String to trim
     * @return Trimmed string
     */
    static std::string Trim(const std::string& str);
    
    static BOOL bInitialized;
    static BOOL bEncrypted;
    static std::string configFilePath;
    static std::map<std::string, ConfigValue> configValues;
    static std::mutex configMutex;
    static std::vector<BYTE> encryptionKey;
};

} // namespace Utils
} // namespace KineToxin

#endif // KINETOXIN_CONFIG_H
