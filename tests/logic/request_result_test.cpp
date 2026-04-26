#include <gtest/gtest.h>

#include "api/request_result.hpp"

class RequestResultTest : public ::testing::Test {};

TEST_F(RequestResultTest, SuccessFactorySetsOkTrue) {
    auto result = API::RequestResult::success();
    EXPECT_TRUE(result.ok);
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
}

TEST_F(RequestResultTest, ErrorFactorySetsOkFalse) {
    auto result = API::RequestResult::error({{"details", "failed"}});
    EXPECT_FALSE(result.ok);
}

TEST_F(RequestResultTest, ErrorMsgFactory) {
    auto result = API::RequestResult::error_msg("timeout");
    EXPECT_FALSE(result.ok);
    EXPECT_EQ(result.body["details"], "timeout");
}

TEST_F(RequestResultTest, ExtractErrorDetailsFromBody) {
    auto result = API::RequestResult::error({{"details", "invalid token"}});
    EXPECT_EQ(result.extractErrorDetails(), "invalid token");
}

TEST_F(RequestResultTest, ExtractErrorDetailsOnSuccessReturnsEmpty) {
    auto result = API::RequestResult::success();
    EXPECT_EQ(result.extractErrorDetails(), "");
}