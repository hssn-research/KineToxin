#ifndef KINETOXIN_CRYPTO_H
#define KINETOXIN_CRYPTO_H

#include <windows.h>
#include <wincrypt.h>
#include <string>
#include <vector>

namespace KineToxin {
namespace Utils {

/**
 * @brief Cryptographic utilities for KineToxin
 * 
 * This class provides encryption, decryption, and hashing functionality
 * using Windows CryptoAPI. All operations use strong cryptographic algorithms.
 */
class Crypto {
public:
    /**
     * @brief Initialize the crypto subsystem
     * @return TRUE if initialization successful, FALSE otherwise
     */
    static BOOL Initialize();
    
    /**
     * @brief Cleanup crypto resources
     */
    static void Cleanup();
    
    /**
     * @brief Encrypt data using AES-256
     * @param data Input data to encrypt
     * @param key Encryption key (32 bytes for AES-256)
     * @param encrypted Output encrypted data
     * @return TRUE if encryption successful, FALSE otherwise
     */
    static BOOL EncryptAES256(const std::vector<BYTE>& data, 
                             const std::vector<BYTE>& key,
                             std::vector<BYTE>& encrypted);
    
    /**
     * @brief Decrypt data using AES-256
     * @param encrypted Input encrypted data
     * @param key Decryption key (32 bytes for AES-256)
     * @param decrypted Output decrypted data
     * @return TRUE if decryption successful, FALSE otherwise
     */
    static BOOL DecryptAES256(const std::vector<BYTE>& encrypted,
                             const std::vector<BYTE>& key,
                             std::vector<BYTE>& decrypted);
    
    /**
     * @brief Generate cryptographically secure random bytes
     * @param size Number of random bytes to generate
     * @param random Output random bytes
     * @return TRUE if generation successful, FALSE otherwise
     */
    static BOOL GenerateRandomBytes(DWORD size, std::vector<BYTE>& random);
    
    /**
     * @brief Calculate SHA-256 hash of data
     * @param data Input data to hash
     * @param hash Output hash (32 bytes)
     * @return TRUE if hashing successful, FALSE otherwise
     */
    static BOOL CalculateSHA256(const std::vector<BYTE>& data,
                               std::vector<BYTE>& hash);
    
    /**
     * @brief XOR encrypt/decrypt data (simple obfuscation)
     * @param data Input data
     * @param key XOR key
     * @param result Output XOR result
     * @return TRUE if operation successful, FALSE otherwise
     */
    static BOOL XORCrypt(const std::vector<BYTE>& data,
                        const std::vector<BYTE>& key,
                        std::vector<BYTE>& result);

private:
    static HCRYPTPROV hCryptProv;
    static BOOL bInitialized;
};

} // namespace Utils
} // namespace KineToxin

#endif // KINETOXIN_CRYPTO_H
