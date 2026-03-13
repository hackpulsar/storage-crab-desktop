#ifndef ALGORITHM_TYPES_H
#define ALGORITHM_TYPES_H

namespace Cryptography {

enum class AlgorithmType {
    AES = 0,
    Hybrid
};

inline AlgorithmType algorithmTypeFromString(const std::string &algorithmType) {
    if (algorithmType == "Hybrid (AES + RSA)")
        return AlgorithmType::Hybrid;
    return AlgorithmType::AES;
}

// Types of AES.
// Numeric values are assigned accordingly to the type's key length in bytes.
enum class AESType {
    AES_128 = 16,
    AES_192 = 24,
    AES_256 = 32,
};

inline AESType AESTypeFromString(const std::string &type) {
    if (type == "AES-128") return AESType::AES_128;
    if (type == "AES-192") return AESType::AES_192;
    return AESType::AES_256;
}

}

#endif //ALGORITHM_TYPES_H
