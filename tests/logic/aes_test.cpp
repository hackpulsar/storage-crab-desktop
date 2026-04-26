#include <gtest/gtest.h>

#include "cryptography/aes.h"
#include "utils/byte_array.hpp"
#include "cryptography/algorithm_types.h"

using namespace Cryptography;
using namespace Cryptography::AES;

class AESTest : public ::testing::Test {};

TEST_F(AESTest, GenerateKey128) {
    auto key = generateKey(AESType::AES_128);
    EXPECT_EQ(key.key.size(), 16);
    EXPECT_EQ(key.iv.size(), 16);
}

TEST_F(AESTest, GenerateKey192) {
    auto key = generateKey(AESType::AES_192);
    EXPECT_EQ(key.key.size(), 24);
    EXPECT_EQ(key.iv.size(), 16);
}

TEST_F(AESTest, GenerateKey256) {
    auto key = generateKey(AESType::AES_256);
    EXPECT_EQ(key.key.size(), 32);
    EXPECT_EQ(key.iv.size(), 16);
}

TEST_F(AESTest, EncryptDecryptRoundtrip128) {
    Utils::ByteArray input = {'h', 'e', 'l', 'l', 'o'};
    auto key = generateKey(AESType::AES_128);
    
    auto encrypted = encrypt(key, input);
    auto decrypted = decrypt(key, encrypted);
    
    EXPECT_EQ(input, decrypted);
}

TEST_F(AESTest, EncryptDecryptRoundtrip192) {
    Utils::ByteArray input = {'h', 'e', 'l', 'l', 'o'};
    auto key = generateKey(AESType::AES_192);
    
    auto encrypted = encrypt(key, input);
    auto decrypted = decrypt(key, encrypted);
    
    EXPECT_EQ(input, decrypted);
}

TEST_F(AESTest, EncryptDecryptRoundtrip256) {
    Utils::ByteArray input = {'h', 'e', 'l', 'l', 'o'};
    auto key = generateKey(AESType::AES_256);
    
    auto encrypted = encrypt(key, input);
    auto decrypted = decrypt(key, encrypted);
    
    EXPECT_EQ(input, decrypted);
}

TEST_F(AESTest, EncryptProducesDifferentOutput) {
    Utils::ByteArray input = {'h', 'e', 'l', 'l', 'o'};
    auto key = generateKey(AESType::AES_256);
    
    auto encrypted = encrypt(key, input);
    
    EXPECT_NE(input, encrypted);
}

TEST_F(AESTest, KeyDataToJSON) {
    auto key = generateKey(AESType::AES_256);
    auto json = key.toJSON();
    
    EXPECT_TRUE(json.contains("type"));
    EXPECT_TRUE(json.contains("AES"));
    EXPECT_TRUE(json["AES"].contains("key"));
    EXPECT_TRUE(json["AES"].contains("iv"));
    EXPECT_TRUE(json["AES"].contains("type"));
    EXPECT_EQ(json["type"], "AES");
    EXPECT_EQ(json["AES"]["type"], AESType::AES_256);
}

TEST_F(AESTest, ParseKeyFromJSON) {
    auto original = generateKey(AESType::AES_256);
    auto json = original.toJSON();
    
    auto parsed = parseKey(json);
    
    EXPECT_EQ(parsed.key.size(), original.key.size());
    EXPECT_EQ(parsed.iv.size(), original.iv.size());
    EXPECT_EQ(parsed.type, AESType::AES_256);
}