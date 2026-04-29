#include <gtest/gtest.h>

#include "utils/byte_array.hpp"

using namespace Utils;

TEST(UtilsTest, byteArrayToHEX) {
    ByteArray input = {0xAB, 0xCD, 0x12, 0x01, 0x0A};
    auto result = toHEX(input);
    EXPECT_EQ(result, "abcd12010a");
}

TEST(UtilsTest, HEXtoByteArray) {
    auto result = toByteArray("abcd12010a");
    EXPECT_EQ(result.size(), 5);
    EXPECT_EQ(result[0], 0xAB);
    EXPECT_EQ(result[1], 0xCD);
    EXPECT_EQ(result[2], 0x12);
    EXPECT_EQ(result[3], 0x01);
    EXPECT_EQ(result[4], 0x0A);
}

TEST(UtilsTest, toByteArrayEmpty) {
    auto result = toByteArray("");
    EXPECT_TRUE(result.empty());
}

TEST(UtilsTest, toHEXEmpty) {
    ByteArray empty = {};
    auto result = toHEX(empty);
    EXPECT_EQ(result, "");
}