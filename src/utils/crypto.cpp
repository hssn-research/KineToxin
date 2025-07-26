#include "crypto.h"
#include <iostream>

namespace KineToxin {
namespace Utils {

// Static member initialization
HCRYPTPROV Crypto::hCryptProv = NULL;
BOOL Crypto::bInitialized = FALSE;

BOOL Crypto::Initialize() {
    if (bInitialized) {
        return TRUE;
    }
    
    // Acquire cryptographic context
    if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_AES, 
                           CRYPT_VERIFYCONTEXT)) {
        return FALSE;
    }
    
    bInitialized = TRUE;
    return TRUE;
}

void Crypto::Cleanup() {
    if (hCryptProv != NULL) {
        CryptReleaseContext(hCryptProv, 0);
        hCryptProv = NULL;
    }
    bInitialized = FALSE;
}

BOOL Crypto::EncryptAES256(const std::vector<BYTE>& data, 
                          const std::vector<BYTE>& key,
                          std::vector<BYTE>& encrypted) {
    if (!bInitialized || key.size() != 32) {
        return FALSE;
    }
    
    HCRYPTKEY hKey = NULL;
    HCRYPTHASH hHash = NULL;
    BOOL bResult = FALSE;
    
    do {
        // Create hash object
        if (!CryptCreateHash(hCryptProv, CALG_SHA_256, 0, 0, &hHash)) {
            break;
        }
        
        // Hash the key
        if (!CryptHashData(hHash, key.data(), static_cast<DWORD>(key.size()), 0)) {
            break;
        }
        
        // Create key from hash
        if (!CryptDeriveKey(hCryptProv, CALG_AES_256, hHash, 0, &hKey)) {
            break;
        }
        
        // Prepare data for encryption
        encrypted = data;
        DWORD dwDataLen = static_cast<DWORD>(encrypted.size());
        DWORD dwBufLen = dwDataLen + 16; // Add padding space
        encrypted.resize(dwBufLen);
        
        // Encrypt the data
        if (!CryptEncrypt(hKey, NULL, TRUE, 0, encrypted.data(), &dwDataLen, dwBufLen)) {
            break;
        }
        
        encrypted.resize(dwDataLen);
        bResult = TRUE;
        
    } while (FALSE);
    
    // Cleanup
    if (hKey) CryptDestroyKey(hKey);
    if (hHash) CryptDestroyHash(hHash);
    
    return bResult;
}

BOOL Crypto::DecryptAES256(const std::vector<BYTE>& encrypted,
                          const std::vector<BYTE>& key,
                          std::vector<BYTE>& decrypted) {
    if (!bInitialized || key.size() != 32) {
        return FALSE;
    }
    
    HCRYPTKEY hKey = NULL;
    HCRYPTHASH hHash = NULL;
    BOOL bResult = FALSE;
    
    do {
        // Create hash object
        if (!CryptCreateHash(hCryptProv, CALG_SHA_256, 0, 0, &hHash)) {
            break;
        }
        
        // Hash the key
        if (!CryptHashData(hHash, key.data(), static_cast<DWORD>(key.size()), 0)) {
            break;
        }
        
        // Create key from hash
        if (!CryptDeriveKey(hCryptProv, CALG_AES_256, hHash, 0, &hKey)) {
            break;
        }
        
        // Prepare data for decryption
        decrypted = encrypted;
        DWORD dwDataLen = static_cast<DWORD>(decrypted.size());
        
        // Decrypt the data
        if (!CryptDecrypt(hKey, NULL, TRUE, 0, decrypted.data(), &dwDataLen)) {
            break;
        }
        
        decrypted.resize(dwDataLen);
        bResult = TRUE;
        
    } while (FALSE);
    
    // Cleanup
    if (hKey) CryptDestroyKey(hKey);
    if (hHash) CryptDestroyHash(hHash);
    
    return bResult;
}

BOOL Crypto::GenerateRandomBytes(DWORD size, std::vector<BYTE>& random) {
    if (!bInitialized) {
        return FALSE;
    }
    
    random.resize(size);
    return CryptGenRandom(hCryptProv, size, random.data());
}

BOOL Crypto::CalculateSHA256(const std::vector<BYTE>& data,
                            std::vector<BYTE>& hash) {
    if (!bInitialized) {
        return FALSE;
    }
    
    HCRYPTHASH hHash = NULL;
    BOOL bResult = FALSE;
    
    do {
        // Create hash object
        if (!CryptCreateHash(hCryptProv, CALG_SHA_256, 0, 0, &hHash)) {
            break;
        }
        
        // Hash the data
        if (!CryptHashData(hHash, data.data(), static_cast<DWORD>(data.size()), 0)) {
            break;
        }
        
        // Get hash size
        DWORD dwHashLen = 32; // SHA-256 is always 32 bytes
        hash.resize(dwHashLen);
        
        // Get the hash value
        if (!CryptGetHashParam(hHash, HP_HASHVAL, hash.data(), &dwHashLen, 0)) {
            break;
        }
        
        bResult = TRUE;
        
    } while (FALSE);
    
    // Cleanup
    if (hHash) CryptDestroyHash(hHash);
    
    return bResult;
}

BOOL Crypto::XORCrypt(const std::vector<BYTE>& data,
                     const std::vector<BYTE>& key,
                     std::vector<BYTE>& result) {
    if (key.empty()) {
        return FALSE;
    }
    
    result.resize(data.size());
    
    for (size_t i = 0; i < data.size(); ++i) {
        result[i] = data[i] ^ key[i % key.size()];
    }
    
    return TRUE;
}

} // namespace Utils
} // namespace KineToxin
