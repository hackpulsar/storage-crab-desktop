#include "cryptography/encryption_service.h"

#include "cryptography/aes.h"
#include "cryptography/rsa.h"

namespace Cryptography {

EncryptionService::Result EncryptionService::Encrypt(
    const std::string& content,
    const std::string& fileName,
    AESType aesType,
    AlgorithmType algorithmType,
    size_t rsaKeySize,
    bool encryptName
)
{
    Result result;

    auto AESconfig = AES::generateKey(aesType);
    result.encryptedContent = AES::encrypt(AESconfig, content);
    result.encryptedFileName = fileName;

    // Encrypt name if checked
    if (encryptName)
        result.encryptedFileName = Utils::toHEX(AES::encrypt(AESconfig, fileName));

    result.config = AESconfig.toJSON();

    switch (algorithmType) {
        // In a hybrid form, AES is used for file contents and filename,
        // RSA is for encrypting an AES key.
        case AlgorithmType::Hybrid: {
            auto RSAconfig = RSA::generateKey(rsaKeySize);
            auto encryptedKey = RSA::encrypt(RSAconfig, Utils::toHEX(AESconfig.key));

            result.config["type"] = "hybrid";
            result.config["AES"]["key"] = Utils::toHEX(encryptedKey);
            result.config["RSA"]["private_key"] = RSA::keyToString(RSAconfig.keyPair, true);
            result.config["RSA"]["public_key"] = RSA::keyToString(RSAconfig.keyPair, false);
            
            break;
        }
        default: break;
    }

    return result;
}

}