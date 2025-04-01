#ifndef ALGORITHM_TYPES_H
#define ALGORITHM_TYPES_H

namespace Cryptography {

enum class AlgorithmType {
    AES = 0,
    RSA,
    ECC
};

inline AlgorithmType algorithmTypeFromString(const std::string &algorithmType) {
    if (algorithmType == "RSA") return AlgorithmType::RSA;
    if (algorithmType == "ECC") return AlgorithmType::ECC;
    return AlgorithmType::AES; // AES by default
}

}

#endif //ALGORITHM_TYPES_H
