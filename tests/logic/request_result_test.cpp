#include <gtest/gtest.h>

#include "api/request_result.hpp"

class RequestResultTest : public ::testing::Test {};

TEST_F(RequestResultTest, SuccessEmpty) {
    auto result = API::RequestResult::success();
    EXPECT_TRUE(result.ok);
    EXPECT_EQ(result.extractErrorDetails(), "");
}

TEST_F(RequestResultTest, SuccessWithBody) {
    auto result = API::RequestResult::success({{"key", "value"}});
    EXPECT_TRUE(result.ok);
    EXPECT_EQ(result.body["key"], "value");
}

TEST_F(RequestResultTest, SuccessWithEmptyBody) {
    auto result = API::RequestResult::success(nlohmann::json::object());
    EXPECT_TRUE(result.ok);
    EXPECT_TRUE(result.body.empty());
    EXPECT_EQ(result.extractErrorDetails(), "");
}

TEST_F(RequestResultTest, ErrorWithEmptyBody) {
    auto result = API::RequestResult::error_msg("");
    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.body["details"], "");
    EXPECT_EQ(result.extractErrorDetails(), "");

    result = API::RequestResult::error(nlohmann::json::object());
    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.extractErrorDetails(), "");
}

TEST_F(RequestResultTest, ErrorWithBody) {
    auto result = API::RequestResult::error_msg("timeout");
    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.body["details"], "timeout");

    result = API::RequestResult::error({{"details", "invalid token"}});
    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.extractErrorDetails(), "invalid token");
}
