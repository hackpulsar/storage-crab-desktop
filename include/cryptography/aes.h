#ifndef AES_H
#define AES_H

#include <nlohmann/json.hpp>

#include "algorithm_types.h"
#include "key.hpp"
#include "utils/byte_array.hpp"

// Default IV size for AES (128-bit)
#define IV_SIZE 16

namespace Cryptography::AES {

using namespace Utils;

// Redefinition of the same type for code clarity purposes
typedef ByteArray AESKey;

struct KeyData final : Key {
    KeyData(AESKey key, ByteArray iv, AESType type);
    ~KeyData() override = default;

    AESKey key;
    ByteArray iv;

    AESType type;

    nlohmann::json toJSON() override;
};

KeyData generateKey(AESType type);

KeyData parseKey(const nlohmann::json& config);

ByteArray encrypt(const KeyData& config, const std::string& input);

std::string decrypt(const KeyData& config, const ByteArray& ciphertext);

}

#endif //AES_H
