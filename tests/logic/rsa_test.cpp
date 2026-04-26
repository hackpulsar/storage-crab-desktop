#include <gtest/gtest.h>

#include "cryptography/rsa.h"
#include "utils/byte_array.hpp"

using namespace Cryptography::RSA;

class RSATest : public ::testing::Test {};

TEST_F(RSATest, GenerateKey1024) {
    auto keys = generateKey(1024);
    EXPECT_NE(keys.keyPair, nullptr);
}

TEST_F(RSATest, GenerateKey2048) {
    auto keys = generateKey(2048);
    EXPECT_NE(keys.keyPair, nullptr);
}

TEST_F(RSATest, GenerateKey4096) {
    auto keys = generateKey(4096);
    EXPECT_NE(keys.keyPair, nullptr);
}

TEST_F(RSATest, EncryptDecryptRoundtrip1024) {
    std::string testData = "secret message";
    Utils::ByteArray input(testData.begin(), testData.end());
    auto keys = generateKey(1024);
    
    auto encrypted = encrypt(keys, input);
    auto decrypted = decrypt(keys, encrypted);
    
    EXPECT_EQ(input, decrypted);
}

TEST_F(RSATest, EncryptDecryptRoundtrip2048) {
    std::string testData = "secret message";
    Utils::ByteArray input(testData.begin(), testData.end());
    auto keys = generateKey(2048);
    
    auto encrypted = encrypt(keys, input);
    auto decrypted = decrypt(keys, encrypted);
    
    EXPECT_EQ(input, decrypted);
}

TEST_F(RSATest, EncryptDecryptRoundtrip4096) {
    std::string testData = "secret message";
    Utils::ByteArray input(testData.begin(), testData.end());
    auto keys = generateKey(4096);
    
    auto encrypted = encrypt(keys, input);
    auto decrypted = decrypt(keys, encrypted);
    
    EXPECT_EQ(input, decrypted);
}

TEST_F(RSATest, EncryptProducesDifferentOutput) {
    std::string testData = "secret message";
    Utils::ByteArray input(testData.begin(), testData.end());
    auto keys = generateKey(2048);
    
    auto encrypted = encrypt(keys, input);
    
    EXPECT_NE(input, encrypted);
}

TEST_F(RSATest, EncryptLargerData) {
    std::string testData(200, 'x'); // Larger than RSA block size
    Utils::ByteArray input(testData.begin(), testData.end());
    auto keys = generateKey(2048);
    
    auto encrypted = encrypt(keys, input);
    auto decrypted = decrypt(keys, encrypted);
    
    EXPECT_EQ(input, decrypted);
}