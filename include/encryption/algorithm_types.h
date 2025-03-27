#ifndef ALGORITHM_TYPES_H
#define ALGORITHM_TYPES_H

namespace Encryption {

enum class AlgorithmType {
    AES = 0,
    RSA,
    ECC
};

inline AlgorithmType algorithmTypeFromString(const std::string &algorithmType) {
    if (algorithmType == "AES") return AlgorithmType::AES;
    if (algorithmType == "RSA") return AlgorithmType::RSA;
    if (algorithmType == "ECC") return AlgorithmType::ECC;
}

}

#endif //ALGORITHM_TYPES_H
