#pragma once

#include <string>

#include "utils/byte_array.hpp"
#include "algorithm_types.h"
#include "result_base.hpp"

namespace Cryptography {

class FileEncryptor final {
public:
    struct Options {
        std::string filePath;
        std::string keyPath;
        AESType aesType;
        AlgorithmType algorithm;
        int rsaKeySize;
        bool encryptFileName;
    };

    struct Result : public ResultBase {
        std::string path;
        std::string fileName;
    };

    static Result encryptFile(const Options& options);

private:
    // @returns File contents on successfull read, empty string otherwise
    static std::string readFile(const std::string& path);

    static bool writeFile(const std::string& path, const Utils::ByteArray& data);

    static std::string buildOutputPath(const std::string& path, const std::string& fileName);

};

}
