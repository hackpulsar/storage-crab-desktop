#include <gtest/gtest.h>
#include <fstream>
#include <cstdio>

#include "nlohmann/json.hpp"

#include "cryptography/file_crypto.h"
#include "cryptography/algorithm_types.h"

using namespace Cryptography;

class FileCryptoTest : public ::testing::Test {
protected:
    void SetUp() override {
        filePath = "./tmp/test.jpg";
        keyPath = "./tmp/test_key.crbkey";
    }

    void TearDown() override {
        std::remove(keyPath.c_str());
    }

    std::string filePath;
    std::string keyPath;
};

TEST_F(FileCryptoTest, EncryptFileReturnsSuccess) {
    FileCrypto::EncryptOptions options = {
        filePath,
        keyPath,
        AESType::AES_256,
        AlgorithmType::Hybrid,
        2048,
        true
    };

    auto result = FileCrypto::encryptFile(options);

    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(result.path.empty());
    EXPECT_FALSE(result.fileName.empty());
}

TEST_F(FileCryptoTest, EncryptFileSavesConfig) {
    FileCrypto::EncryptOptions options = {
        filePath,
        keyPath,
        AESType::AES_256,
        AlgorithmType::Hybrid,
        2048,
        true
    };

    auto result = FileCrypto::encryptFile(options);
    ASSERT_TRUE(result.ok);

    std::ifstream keyFile(keyPath);
    ASSERT_TRUE(keyFile.is_open());

    nlohmann::json config = nlohmann::json::parse(keyFile, nullptr, false);
    EXPECT_FALSE(config.is_discarded());
}

TEST_F(FileCryptoTest, DecryptFileReturnsSuccess) {
    FileCrypto::EncryptOptions encryptOptions = {
        filePath,
        keyPath,
        AESType::AES_256,
        AlgorithmType::Hybrid,
        2048,
        true
    };

    auto encryptResult = FileCrypto::encryptFile(encryptOptions);
    ASSERT_TRUE(encryptResult.ok);

    FileCrypto::DecryptOptions decryptOptions = {
        encryptResult.path,
        keyPath,
        true
    };

    auto decryptResult = FileCrypto::decryptFile(decryptOptions);

    EXPECT_TRUE(decryptResult.ok);
    EXPECT_FALSE(decryptResult.path.empty());
}

TEST_F(FileCryptoTest, EncryptDecryptRoundtrip) {
    std::string originalFilePath = filePath;

    std::string encryptedPath = "";

    {
        FileCrypto::EncryptOptions options = {
            originalFilePath,
            keyPath,
            AESType::AES_256,
            AlgorithmType::Hybrid,
            2048,
            true
        };
        auto result = FileCrypto::encryptFile(options);
        ASSERT_TRUE(result.ok);
        encryptedPath = result.path;
    }

    {
        FileCrypto::DecryptOptions options = {
            encryptedPath,
            keyPath,
            true
        };
        auto result = FileCrypto::decryptFile(options);
        ASSERT_TRUE(result.ok);
    }
}

TEST_F(FileCryptoTest, EncryptFileWithAESAlgorithm) {
    FileCrypto::EncryptOptions options = {
        filePath,
        keyPath,
        AESType::AES_256,
        AlgorithmType::AES,
        2048,
        true
    };

    auto result = FileCrypto::encryptFile(options);

    EXPECT_TRUE(result.ok);
    EXPECT_FALSE(result.path.empty());
}