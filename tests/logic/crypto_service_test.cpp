#include <gtest/gtest.h>

#include "cryptography/crypto_service.h"
#include "cryptography/algorithm_types.h"
#include "utils/byte_array.hpp"

using namespace Cryptography;

class CryptoServiceTest : public ::testing::Test {};

TEST_F(CryptoServiceTest, EncryptReturnsValidResult) {
    Utils::ByteArray content = {'t', 'e', 's', 't'};
    auto result = CryptoService::Encrypt(
        content, 
        "test.txt", 
        AESType::AES_256, 
        AlgorithmType::AES, 
        2048, 
        true
    );
    
    EXPECT_FALSE(result.encryptedContent.empty());
    EXPECT_TRUE(result.config.contains("type"));
    EXPECT_TRUE(result.config.contains("AES"));
    EXPECT_TRUE(result.config["AES"].contains("key"));
    EXPECT_TRUE(result.config["AES"].contains("iv"));
    EXPECT_TRUE(result.config["AES"].contains("type"));
    EXPECT_FALSE(result.config.contains("RSA"));
    EXPECT_FALSE(result.encryptedFileName.empty());

    result = CryptoService::Encrypt(
        content, 
        "test.txt", 
        AESType::AES_256, 
        AlgorithmType::AES, 
        2048, 
        false
    );

    EXPECT_FALSE(result.encryptedContent.empty());
    EXPECT_TRUE(result.config.contains("type"));
    EXPECT_TRUE(result.config.contains("AES"));
    EXPECT_TRUE(result.config["AES"].contains("key"));
    EXPECT_TRUE(result.config["AES"].contains("iv"));
    EXPECT_TRUE(result.config["AES"].contains("type"));
    EXPECT_FALSE(result.config.contains("RSA"));
    EXPECT_EQ(result.encryptedFileName, "test.txt");
}

TEST_F(CryptoServiceTest, EncryptHybridReturnsValidResult) {
    Utils::ByteArray content = {'t', 'e', 's', 't'};
    auto result = CryptoService::Encrypt(
        content, 
        "test.txt", 
        AESType::AES_256, 
        AlgorithmType::Hybrid, 
        2048, 
        true
    );
    
    EXPECT_FALSE(result.encryptedContent.empty());
    EXPECT_TRUE(result.config.contains("type"));
    EXPECT_TRUE(result.config.contains("AES"));
    EXPECT_TRUE(result.config["AES"].contains("key"));
    EXPECT_TRUE(result.config["AES"].contains("iv"));
    EXPECT_TRUE(result.config["AES"].contains("type"));
    EXPECT_TRUE(result.config.contains("RSA"));
    EXPECT_TRUE(result.config["RSA"].contains("private_key"));
    EXPECT_TRUE(result.config["RSA"].contains("public_key"));
    EXPECT_FALSE(result.encryptedFileName.empty());

    result = CryptoService::Encrypt(
        content, 
        "test.txt", 
        AESType::AES_256, 
        AlgorithmType::Hybrid, 
        2048, 
        false
    );
    
    EXPECT_FALSE(result.encryptedContent.empty());
    EXPECT_TRUE(result.config.contains("type"));
    EXPECT_TRUE(result.config.contains("AES"));
    EXPECT_TRUE(result.config["AES"].contains("key"));
    EXPECT_TRUE(result.config["AES"].contains("iv"));
    EXPECT_TRUE(result.config["AES"].contains("type"));
    EXPECT_TRUE(result.config.contains("RSA"));
    EXPECT_TRUE(result.config["RSA"].contains("private_key"));
    EXPECT_TRUE(result.config["RSA"].contains("public_key"));
    EXPECT_EQ(result.encryptedFileName, "test.txt");
}

TEST_F(CryptoServiceTest, EncryptDecryptRoundtrip) {
    Utils::ByteArray original = {'t', 'e', 's', 't'};
    auto encryptResult = CryptoService::Encrypt(
        original, 
        "test.txt", 
        AESType::AES_256, 
        AlgorithmType::AES, 
        2048, 
        false
    );
    
    nlohmann::json config = encryptResult.config;
    auto decryptResult = CryptoService::Decrypt(
        encryptResult.encryptedContent,
        encryptResult.encryptedFileName,
        config,
        false
    );
    
    EXPECT_EQ(decryptResult.content, original);
    EXPECT_EQ(decryptResult.decryptedFileName, "test.txt");
}

TEST_F(CryptoServiceTest, EncryptDecryptRoundtripWithNameEncryption) {
    Utils::ByteArray original = {'h', 'e', 'l', 'l', 'o'};
    auto encryptResult = CryptoService::Encrypt(
        original, 
        "test.txt", 
        AESType::AES_256, 
        AlgorithmType::Hybrid, 
        2048, 
        true
    );
    
    nlohmann::json config = encryptResult.config;
    auto decryptResult = CryptoService::Decrypt(
        encryptResult.encryptedContent,
        encryptResult.encryptedFileName,
        config,
        true
    );
    
    EXPECT_EQ(decryptResult.content, original);
    EXPECT_EQ(decryptResult.decryptedFileName, "test.txt");
}