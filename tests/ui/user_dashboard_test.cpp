#include <gtest/gtest.h>

#include <QTest>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <QPointer>
#include <QTimer>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QFileDialog>

#include "windows/user_dashboard.h"
#include "windows/login_window.h"

class UserDashboardTest : public ::testing::Test {
protected:
    void SetUp() override {
        window = new UserDashboard("some_username", false);
        window->setAttribute(Qt::WA_DeleteOnClose);

        logoutButton = window->findChild<QPushButton*>("logoutButton");
        middlePanelLayout = window->findChild<QVBoxLayout*>("middlePanelLayout");
    }

    QPointer<UserDashboard> window;
    QPushButton* logoutButton;
    QVBoxLayout* middlePanelLayout;

};

TEST_F(UserDashboardTest, HasControls) {
    // Labels
    EXPECT_NE(window->findChild<QLabel*>("titleLabel"), nullptr);
    EXPECT_NE(window->findChild<QLabel*>("usernameLabel"), nullptr);

    // File panels layout
    EXPECT_NE(middlePanelLayout, nullptr);

    // Buttons
    EXPECT_NE(logoutButton, nullptr);
    EXPECT_NE(window->findChild<QPushButton*>("uploadButton"), nullptr);
    EXPECT_NE(window->findChild<QPushButton*>("decryptButton"), nullptr);
    EXPECT_NE(window->findChild<QPushButton*>("downloadSharedButton"), nullptr);
}

TEST_F(UserDashboardTest, PopulatesUsername) {
    EXPECT_EQ(window->findChild<QLabel*>("usernameLabel")->text(), "some_username");
}

TEST_F(UserDashboardTest, UploadedFilesAppear) {
    EXPECT_EQ(middlePanelLayout->count(), 1); // 1 because spacer is already there

    for (size_t i = 0; i < 5; i++) {
        window->addFile(FileData {
            .name = "some_file.txt",
            .path = "/some/path",
            .size = 67,
            .id = i
        });
    }

    EXPECT_EQ(middlePanelLayout->count(), 6);
}

TEST_F(UserDashboardTest, LogoutClosesWithWarning) {
    bool messageBoxShown = false;

    QTimer::singleShot(100, [&]() {
        QWidgetList widgets = QApplication::topLevelWidgets();
        for (QWidget* widget : widgets) {
            if (QMessageBox* mb = qobject_cast<QMessageBox*>(widget)) {
                messageBoxShown = true;
                EXPECT_EQ(mb->text(), "Are you sure you want to logout?");
                mb->button(QMessageBox::Yes)->click();
            }
        }
    });

    QTest::mouseClick(logoutButton, Qt::LeftButton);
    QTest::qWait(100);
    
    EXPECT_TRUE(messageBoxShown);
    EXPECT_TRUE(window.isNull());
}

TEST_F(UserDashboardTest, LoginWindowShowsOnClose) {
    window->close();
    QTest::qWait(100);

    bool loginWindowFound = false;
    for (QWidget* widget : QApplication::topLevelWidgets()) {
        if (qobject_cast<LoginWindow*>(widget)) {
            loginWindowFound = true;
            widget->close(); // cleanup
            break;
        }
    }

    EXPECT_TRUE(loginWindowFound);
}
