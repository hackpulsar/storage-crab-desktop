#include <gtest/gtest.h>
#include <QTest>

#include "windows/login_window.h"

class LoginWindowTest : public ::testing::Test {
protected:
    void SetUp() override {}

};

// === TEST CASES ===
// ErrorMessageShows
// EmptyEmail
// EmptyPassword
// LoginButtonResets

TEST_F(LoginWindowTest, SanityTest) {
    EXPECT_TRUE(true);
}