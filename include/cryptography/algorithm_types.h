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

}

#endif //ALGORITHM_TYPES_H
