#include "cryptography/crypto_service.h"

#include "cryptography/aes.h"
#include "cryptography/rsa.h"

namespace Cryptography {

CryptoService::EncryptResult CryptoService::Encrypt(
    const std::string& content,
    const std::string& fileName,
    AESType aesType,
    AlgorithmType algorithmType,
    size_t rsaKeySize,
    bool encryptName
)
{
    EncryptResult result;

    auto AESconfig = AES::generateKey(aesType);
    result.encryptedContent = AES::encrypt(AESconfig, content);
    result.encryptedFileName = fileName;

    // Encrypt name if checked
    if (encryptName)
        result.encryptedFileName = Utils::toHEX(AES::encrypt(AESconfig, fileName));

    result.config = AESconfig.toJSON();

    // In a hybrid form, AES is used for file contents and filename,
    // RSA is for encrypting an AES key.
    if (algorithmType == AlgorithmType::Hybrid) {
        auto RSAconfig = RSA::generateKey(rsaKeySize);
        auto encryptedKey = RSA::encrypt(RSAconfig, Utils::toHEX(AESconfig.key));

        result.config["type"] = "hybrid";
        result.config["AES"]["key"] = Utils::toHEX(encryptedKey);
        result.config["RSA"]["private_key"] = RSA::keyToString(RSAconfig.keyPair, true);
        result.config["RSA"]["public_key"] = RSA::keyToString(RSAconfig.keyPair, false);
    }

    return result;
}

CryptoService::DecryptResult CryptoService::Decrypt(
    const Utils::ByteArray& content,
    const std::string& fileName,
    nlohmann::json& config,
    bool decryptName
)
{
    DecryptResult result;

    // If encryption is hybrid, decrypt the AES key first
    if (algorithmTypeFromString(config.at("type")) == AlgorithmType::Hybrid) {
        Utils::ByteArray encryptedAESKey = Utils::toByteArray(config.at("AES").at("key"));   

        auto RSAconfig = RSA::parseKey(config);
        auto decryptedKey = RSA::decrypt(RSAconfig, encryptedAESKey);

        // Update in config to further pass to AES parser
        config["AES"]["key"] = decryptedKey;
    }

    // Decrypt content
    auto AESconfig = AES::parseKey(config);
    result.content = AES::decrypt(AESconfig, content);

    result.decryptedFileName = fileName;
    if (decryptName)
        result.decryptedFileName = AES::decrypt(AESconfig, Utils::toByteArray(fileName));

    return result;
}

}