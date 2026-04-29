#include <gtest/gtest.h>

#include <QTest>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

#include "windows/login_window.h"

class LoginWindowTest : public ::testing::Test {
protected:
    void SetUp() override {
        qputenv("TOKEN_OBTAIN_URL", "itdoesntexist");

        window = std::make_unique<LoginWindow>();

        emailLineEdit = window->findChild<QLineEdit*>("emailLineEdit");
        passwordLineEdit = window->findChild<QLineEdit*>("passwordLineEdit");
        loginButton = window->findChild<QPushButton*>("loginButton");
        errorLabel = window->findChild<QLabel*>("errorLabel");
    }

    std::unique_ptr<LoginWindow> window;

    QLineEdit *emailLineEdit, *passwordLineEdit;
    QPushButton* loginButton;
    QLabel* errorLabel;

};

TEST_F(LoginWindowTest, HasControls) {
    EXPECT_NE(emailLineEdit, nullptr);
    EXPECT_NE(passwordLineEdit, nullptr);
    EXPECT_NE(loginButton, nullptr);
    EXPECT_NE(errorLabel, nullptr);
}

TEST_F(LoginWindowTest, PasswordHidden) {
    EXPECT_EQ(passwordLineEdit->echoMode(), QLineEdit::EchoMode::Password);
}

TEST_F(LoginWindowTest, CorrectTitle) {
    QLabel* titleLabel = window->findChild<QLabel*>("titleLabel");
    QLabel* subtitleLabel = window->findChild<QLabel*>("subtitleLabel");

    EXPECT_EQ(titleLabel->text().toStdString(), "Storage Crab");
    EXPECT_EQ(subtitleLabel->text().toStdString(), "\360\237\246\200");
}

TEST_F(LoginWindowTest, TimeoutMessageShows) {
    QTest::mouseClick(loginButton, Qt::LeftButton);
    QTest::qWait(100); // wait for request to fail
    EXPECT_EQ(errorLabel->text().toStdString(), "Request timed out");
}

TEST_F(LoginWindowTest, EmptyFieldsError) {
    emailLineEdit->setText("");
    passwordLineEdit->setText("");

    QTest::mouseClick(loginButton, Qt::LeftButton);
    QTest::qWait(100); // wait for request to fail
    EXPECT_EQ(errorLabel->text().toStdString(), "Please fill the fields below");
}

TEST_F(LoginWindowTest, LoginButtonResets) {
    QTest::mouseClick(loginButton, Qt::LeftButton);
    EXPECT_TRUE(loginButton->text().isEmpty());
    QTest::qWait(100); // wait for request to fail
    EXPECT_EQ(loginButton->text().toStdString(), "Login");
}
