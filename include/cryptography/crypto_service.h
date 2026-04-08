#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include "utils/byte_array.hpp"
#include "cryptography/algorithm_types.h"

namespace Cryptography {

class CryptoService final {
public:
    struct EncryptResult {
        Utils::ByteArray encryptedContent;
        std::string encryptedFileName;
        nlohmann::json config;
    };

    struct DecryptResult {
        Utils::ByteArray content;
        std::string decryptedFileName;
    };

    static EncryptResult Encrypt(
        const Utils::ByteArray& content,
        const std::string& fileName,
        AESType aesType,
        AlgorithmType algorithmType,
        size_t rsaKeySize,
        bool encryptName
    );

    static DecryptResult Decrypt(
        const Utils::ByteArray& content,
        const std::string& fileName,
        nlohmann::json& config,
        bool decryptName
    );
};

}
