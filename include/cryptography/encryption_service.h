#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include "utils/byte_array.hpp"
#include "cryptography/algorithm_types.h"

namespace Cryptography {

class EncryptionService final {
public:
    struct Result {
        Utils::ByteArray encryptedContent;
        std::string encryptedFileName;
        nlohmann::json config;
    };

    static Result Encrypt(
        const std::string& content,
        const std::string& fileName,
        AESType aesType,
        AlgorithmType algorithmType,
        size_t rsaKeySize,
        bool encryptname
    );
};

}
