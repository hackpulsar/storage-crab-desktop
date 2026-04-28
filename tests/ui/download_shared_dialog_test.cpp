#include <gtest/gtest.h>

#include <QTest>
#include <QTimer>
#include <QMessageBox>
#include <QLineEdit>
#include <QPushButton>

#include "windows/download_shared_dialog.h"

class DownloadSharedDialogTest : public ::testing::Test {
protected:
    void SetUp() override {
        dialog = std::make_unique<DownloadSharedDialog>();
    }

    std::unique_ptr<DownloadSharedDialog> dialog;
};

TEST_F(DownloadSharedDialogTest, HasControls) {
    QLineEdit* codeLineEdit = dialog->findChild<QLineEdit*>("codeLineEdit");
    QPushButton* downloadButton = dialog->findChild<QPushButton*>("downloadButton");
    
    ASSERT_NE(codeLineEdit, nullptr);
    ASSERT_NE(downloadButton, nullptr);
}

TEST_F(DownloadSharedDialogTest, DownloadButtonEmptyCode) {
    QLineEdit* codeLineEdit = dialog->findChild<QLineEdit*>("codeLineEdit");
    QPushButton* downloadButton = dialog->findChild<QPushButton*>("downloadButton");
    ASSERT_NE(codeLineEdit, nullptr);
    ASSERT_NE(downloadButton, nullptr);

    codeLineEdit->setText("");

    bool messageBoxShown = false;

    QTimer::singleShot(100, [&]() {
        QWidgetList widgets = QApplication::topLevelWidgets();
        for (QWidget* widget : widgets) {
            if (QMessageBox* mb = qobject_cast<QMessageBox*>(widget)) {
                messageBoxShown = true;
                EXPECT_EQ(mb->text(), "Please fill all the fields");
                mb->close();
            }
        }
    });

    QTest::mouseClick(downloadButton, Qt::LeftButton);

    EXPECT_TRUE(messageBoxShown);
}