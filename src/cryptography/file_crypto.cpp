#include "cryptography/file_crypto.h"

#include <fstream>

#include <QFileInfo>
#include <QDir>

#include "cryptography/crypto_service.h"

namespace Cryptography {

FileCrypto::Result FileCrypto::encryptFile(const EncryptOptions& opts) {
    const std::string content = readFile(opts.filePath);

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
        return Result { { .ok = false } }; // empty failure
    }

    // Save config
    std::ofstream configFile(opts.keyPath);
    configFile << result.config.dump();

    // Build output path and write
    const std::string outPath = buildOutputPath(opts.filePath, result.encryptedFileName + "." + fileExtension);
    writeFile(outPath, result.encryptedContent);

    return Result {
        { .ok = true },
        .path       = outPath, 
        .fileName   = result.encryptedFileName + "." + fileExtension
    };
}

FileCrypto::Result FileCrypto::decryptFile(const DecryptOptions& opts) {
    return {};
}

std::string FileCrypto::readFile(const std::string& path) {
    std::fstream sourceFile(path, std::fstream::in | std::fstream::binary);
    if (!sourceFile.is_open()) return "";

    std::ostringstream content;
    content << sourceFile.rdbuf();

    return content.str();
}

bool FileCrypto::writeFile(const std::string& path, const Utils::ByteArray& data) {
    std::ofstream encryptedFile(path, std::ios::binary);
    if (!encryptedFile.is_open()) return false;

    encryptedFile.write(reinterpret_cast<const std::ostream::char_type*>(data.data()), data.size());
    return true;
}

std::string FileCrypto::buildOutputPath(const std::string& path, const std::string& fileName) {
    const QString dir = QFileInfo(QString::fromStdString(path)).absolutePath();
    return QDir(dir).filePath(QString::fromStdString(fileName + ".enc")).toStdString();
}

}