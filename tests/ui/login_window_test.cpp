#include <gtest/gtest.h>

#include <QTest>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QPointer>

#include "windows/login_window.h"

class LoginWindowTest : public ::testing::Test {
protected:
    void SetUp() override {
        qputenv("TOKEN_OBTAIN_URL", "itdoesntexist");

        window = new LoginWindow;
        window->setAttribute(Qt::WA_DeleteOnClose);

        emailLineEdit = window->findChild<QLineEdit*>("emailLineEdit");
        passwordLineEdit = window->findChild<QLineEdit*>("passwordLineEdit");
        loginButton = window->findChild<QPushButton*>("loginButton");
        errorLabel = window->findChild<QLabel*>("errorLabel");
        registerButton = window->findChild<QPushButton*>("registerButton");
    }

    QPointer<LoginWindow> window;

    QLineEdit *emailLineEdit, *passwordLineEdit;
    QPushButton *loginButton, *registerButton;
    QLabel* errorLabel;

};

TEST_F(LoginWindowTest, HasControls) {
    EXPECT_NE(emailLineEdit, nullptr);
    EXPECT_NE(passwordLineEdit, nullptr);
    EXPECT_NE(loginButton, nullptr);
    EXPECT_NE(errorLabel, nullptr);
    EXPECT_NE(registerButton, nullptr);
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
    emailLineEdit->setText("test@test.com");
    passwordLineEdit->setText("strong_password");

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

TEST_F(LoginWindowTest, RegisterButtonClosesCurrent) {
    QTest::mouseClick(registerButton, Qt::LeftButton);
    QTest::qWait(100); // wait for event to process
    EXPECT_TRUE(window.isNull());
}
