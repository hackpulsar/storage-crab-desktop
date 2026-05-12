#include <gtest/gtest.h>

#include <QTest>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

#include "windows/register_window.h"

class RegisterWindowTest : public ::testing::Test {
protected:
    void SetUp() override {
        qputenv("REGISTER_URL", "itdoesntexist");
        
        window = std::make_unique<RegisterWindow>();

        emailLineEdit = window->findChild<QLineEdit*>("emailLineEdit");
        usernameLineEdit = window->findChild<QLineEdit*>("usernameLineEdit");
        passwordLineEdit = window->findChild<QLineEdit*>("passwordLineEdit");
        passwordRepeatLineEdit = window->findChild<QLineEdit*>("passwordRepeatLineEdit");
        errorLabel = window->findChild<QLabel*>("errorLabel");
        registerButton = window->findChild<QPushButton*>("registerButton");
    }

    std::unique_ptr<RegisterWindow> window;

    QLineEdit *emailLineEdit, *usernameLineEdit, *passwordLineEdit, *passwordRepeatLineEdit;
    QPushButton *loginButton, *registerButton;
    QLabel* errorLabel;
};

TEST_F(RegisterWindowTest, HasControls) {
    EXPECT_NE(emailLineEdit, nullptr);
    EXPECT_NE(usernameLineEdit, nullptr);
    EXPECT_NE(passwordLineEdit, nullptr);
    EXPECT_NE(passwordRepeatLineEdit, nullptr);
    EXPECT_NE(errorLabel, nullptr);
    EXPECT_NE(registerButton, nullptr);
}

TEST_F(RegisterWindowTest, HasPlaceholderText) {
    EXPECT_EQ(emailLineEdit->placeholderText().toStdString(), "Email");
    EXPECT_EQ(usernameLineEdit->placeholderText().toStdString(), "Username");
    EXPECT_EQ(passwordLineEdit->placeholderText().toStdString(), "Password");
    EXPECT_EQ(passwordRepeatLineEdit->placeholderText().toStdString(), "Repeat password");
}

TEST_F(RegisterWindowTest, PasswordHidden) {
    EXPECT_EQ(passwordLineEdit->echoMode(), QLineEdit::EchoMode::Password);
    EXPECT_EQ(passwordRepeatLineEdit->echoMode(), QLineEdit::EchoMode::Password);
}

TEST_F(RegisterWindowTest, TimeoutMessageShows) {
    emailLineEdit->setText("test@test.com");
    usernameLineEdit->setText("test");
    passwordLineEdit->setText("strong_password");
    passwordRepeatLineEdit->setText("strong_password");


    QTest::mouseClick(registerButton, Qt::LeftButton);
    QTest::qWait(100); // wait for request to fail
    EXPECT_EQ(errorLabel->text().toStdString(), "Request timed out");
}

TEST_F(RegisterWindowTest, PasswordsDontMatch) {
    emailLineEdit->setText("test@test.com");
    usernameLineEdit->setText("test");
    passwordLineEdit->setText("strong_password");
    passwordRepeatLineEdit->setText("other_password");

    QTest::mouseClick(registerButton, Qt::LeftButton);
    QTest::qWait(100); // wait for request to fail
    EXPECT_EQ(errorLabel->text().toStdString(), "Passwords don't match");
}

TEST_F(RegisterWindowTest, EmptyFieldsError) {
    QTest::mouseClick(registerButton, Qt::LeftButton);
    QTest::qWait(100); // wait for request to fail
    EXPECT_EQ(errorLabel->text().toStdString(), "Please fill the fields below");
}

TEST_F(RegisterWindowTest, RegisterButtonResets) {
    emailLineEdit->setText("test@test.com");
    usernameLineEdit->setText("test");
    passwordLineEdit->setText("strong_password");
    passwordRepeatLineEdit->setText("strong_password");

    registerButton->click();
    EXPECT_TRUE(registerButton->text().isEmpty());
    
    QTest::qWait(200);
    EXPECT_EQ(registerButton->text().toStdString(), "Register");
}
