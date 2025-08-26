// Test cases for cryptographic utilities
#include "../src/utils/crypto.h"

bool test_crypto_initialization() {
    // Test crypto system initialization
    TEST_ASSERT(KineToxin::Utils::Crypto::Initialize() == TRUE);
    
    // Cleanup
    KineToxin::Utils::Crypto::Cleanup();
    
    return true;
}

bool test_crypto_random_generation() {
    TEST_ASSERT(KineToxin::Utils::Crypto::Initialize() == TRUE);
    
    std::vector<BYTE> random1, random2;
    
    // Test random byte generation
    TEST_ASSERT(KineToxin::Utils::Crypto::GenerateRandomBytes(32, random1) == TRUE);
    TEST_ASSERT(random1.size() == 32);
    
    // Generate another set and ensure they're different
    TEST_ASSERT(KineToxin::Utils::Crypto::GenerateRandomBytes(32, random2) == TRUE);
    TEST_ASSERT(random1 != random2);
    
    KineToxin::Utils::Crypto::Cleanup();
    return true;
}

bool test_crypto_aes_encryption() {
    TEST_ASSERT(KineToxin::Utils::Crypto::Initialize() == TRUE);
    
    // Generate key and test data
    std::vector<BYTE> key, testData, encrypted, decrypted;
    TEST_ASSERT(KineToxin::Utils::Crypto::GenerateRandomBytes(32, key) == TRUE);
    
    std::string testString = "Hello, KineToxin!";
    testData.assign(testString.begin(), testString.end());
    
    // Test encryption
    TEST_ASSERT(KineToxin::Utils::Crypto::EncryptAES256(testData, key, encrypted) == TRUE);
    TEST_ASSERT(!encrypted.empty());
    TEST_ASSERT(encrypted != testData);
    
    // Test decryption
    TEST_ASSERT(KineToxin::Utils::Crypto::DecryptAES256(encrypted, key, decrypted) == TRUE);
    TEST_ASSERT(decrypted == testData);
    
    KineToxin::Utils::Crypto::Cleanup();
    return true;
}

bool test_crypto_sha256_hashing() {
    TEST_ASSERT(KineToxin::Utils::Crypto::Initialize() == TRUE);
    
    std::string testString = "KineToxin Test Data";
    std::vector<BYTE> testData(testString.begin(), testString.end());
    std::vector<BYTE> hash1, hash2;
    
    // Test hashing
    TEST_ASSERT(KineToxin::Utils::Crypto::CalculateSHA256(testData, hash1) == TRUE);
    TEST_ASSERT(hash1.size() == 32); // SHA-256 produces 32-byte hash
    
    // Test consistency
    TEST_ASSERT(KineToxin::Utils::Crypto::CalculateSHA256(testData, hash2) == TRUE);
    TEST_ASSERT(hash1 == hash2);
    
    KineToxin::Utils::Crypto::Cleanup();
    return true;
}

bool test_crypto_xor_operations() {
    std::string testString = "XOR Test Data";
    std::string keyString = "SecretKey";
    
    std::vector<BYTE> testData(testString.begin(), testString.end());
    std::vector<BYTE> key(keyString.begin(), keyString.end());
    std::vector<BYTE> xorResult, doubleXor;
    
    // Test XOR encryption
    TEST_ASSERT(KineToxin::Utils::Crypto::XORCrypt(testData, key, xorResult) == TRUE);
    TEST_ASSERT(xorResult != testData);
    
    // Test XOR decryption (XOR is symmetric)
    TEST_ASSERT(KineToxin::Utils::Crypto::XORCrypt(xorResult, key, doubleXor) == TRUE);
    TEST_ASSERT(doubleXor == testData);
    
    return true;
}
