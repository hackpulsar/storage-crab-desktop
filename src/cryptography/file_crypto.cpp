#include "cryptography/file_crypto.h"

#include <fstream>

#include <QFileInfo>
#include <QDir>

#include "cryptography/crypto_service.h"

namespace Cryptography {

FileCrypto::Result FileCrypto::encryptFile(const EncryptOptions& opts) {
    using namespace Utils;

    const ByteArray content = readFile(opts.filePath);

    QFileInfo fileInfo(QString::fromStdString(opts.filePath));
    const std::string fileName = fileInfo.baseName().toStdString();
    const std::string fileExtension = fileInfo.suffix().toStdString();

    const int rsaKeySize = opts.rsaKeySize;
    CryptoService::EncryptResult result;
    
    try {
        result = CryptoService::Encrypt(
            content, fileName, opts.aesType, opts.algorithm,
            rsaKeySize, opts.encryptFileName
        );
    } catch (std::exception&) {
        return Result { false }; // empty failure
    }

    // Save config
    std::ofstream configFile(opts.keyPath);
    configFile << result.config.dump();

    // Build output path and write
    const std::string outPath = buildOutputPath(opts.filePath, result.encryptedFileName + "." + fileExtension + ".enc");
    writeFile(outPath, result.encryptedContent);

    return Result { true, outPath, result.encryptedFileName + "." + fileExtension };
}

FileCrypto::Result FileCrypto::decryptFile(const DecryptOptions& opts) {
    using namespace Utils;

    const ByteArray content = readFile(opts.filePath);

    std::ifstream ifs(opts.keyPath);
    nlohmann::json config = nlohmann::json::parse(ifs);

    QFileInfo fileInfo(QString::fromStdString(opts.filePath));
    const std::string fileName = fileInfo.baseName().toStdString();
    std::string fileExtension = fileInfo.completeSuffix().toStdString();

    // Trimming '.enc'
    fileExtension.resize(fileExtension.size() - 4);

    CryptoService::DecryptResult result;
    
    try {
        result = CryptoService::Decrypt(content, fileName, config, opts.decryptFileName);
    } catch (std::exception&) {
        return Result { false }; // empty failure
    }

    // Build output path and write
    const std::string outPath = buildOutputPath(opts.filePath, result.decryptedFileName + "." + fileExtension);
    writeFile(outPath, result.content);

    return Result {
        true,
        outPath,
        std::string(result.decryptedFileName.begin(), result.decryptedFileName.end()) + "." + fileExtension
    };
}

Utils::ByteArray FileCrypto::readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) return {};

    // Determine file size
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);

    Utils::ByteArray buffer(size);
    file.read(reinterpret_cast<char*>(buffer.data()), size);

    return buffer;
}

bool FileCrypto::writeFile(const std::string& path, const Utils::ByteArray& data) {
    std::ofstream encryptedFile(path, std::ios::binary);
    if (!encryptedFile.is_open()) return false;

    encryptedFile.write(reinterpret_cast<const std::ostream::char_type*>(data.data()), data.size());
    return true;
}

std::string FileCrypto::buildOutputPath(const std::string& path, const std::string& fileName) {
    const QString dir = QFileInfo(path.c_str()).absolutePath();
    return QDir(dir).filePath(fileName.c_str()).toStdString();
}

}
