#include <gtest/gtest.h>

#include <QTest>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>
#include <QTimer>
#include <QMessageBox>

#include "windows/decrypt_dialog.h"

class DecryptDialogTest : public ::testing::Test {
protected:
    void SetUp() override {
        dialog = std::make_unique<DecryptDialog>("/path/to/a/file");

        filePathLineEdit = dialog->findChild<QLineEdit*>("filePathLineEdit");
        keyPathLineEdit = dialog->findChild<QLineEdit*>("keyPathLineEdit");
        decryptButton = dialog->findChild<QPushButton*>("decryptButton");
    }

    std::unique_ptr<DecryptDialog> dialog;

    QLineEdit *filePathLineEdit, *keyPathLineEdit;
    QPushButton *decryptButton;
};

TEST_F(DecryptDialogTest, HasControls) {
    QLabel* filePathLabel = dialog->findChild<QLabel*>("filePathLabel");
    QLabel* keyPathLabel = dialog->findChild<QLabel*>("keyPathLabel");
    QLabel* decryptNameLabel = dialog->findChild<QLabel*>("decryptNameLabel");
    QCheckBox* decryptNameCheckBox = dialog->findChild<QCheckBox*>("decryptNameCheckBox");
    QPushButton* chooseFilePathButton = dialog->findChild<QPushButton*>("chooseFilePathButton");
    QPushButton* chooseKeyPathButton = dialog->findChild<QPushButton*>("chooseKeyPathButton");
    QPushButton *cancelButton = dialog->findChild<QPushButton*>("cancelButton");

    EXPECT_NE(filePathLabel, nullptr);
    EXPECT_NE(keyPathLabel, nullptr);
    EXPECT_NE(decryptNameLabel, nullptr);
    EXPECT_NE(filePathLineEdit, nullptr);
    EXPECT_NE(keyPathLineEdit, nullptr);
    EXPECT_NE(decryptNameCheckBox, nullptr);
    EXPECT_NE(chooseFilePathButton, nullptr);
    EXPECT_NE(chooseKeyPathButton, nullptr);
    EXPECT_NE(cancelButton, nullptr);
    EXPECT_NE(decryptButton, nullptr);
}

TEST_F(DecryptDialogTest, EmptyFieldsError) {
    filePathLineEdit->setText("");
    keyPathLineEdit->setText("/some/path");
    
    bool messageBoxShown = false;

    QTimer::singleShot(100, [&]() {
        QWidgetList widgets = QApplication::topLevelWidgets();
        for (QWidget* widget : widgets) {
            if (QMessageBox* mb = qobject_cast<QMessageBox*>(widget)) {
                messageBoxShown = true;
                EXPECT_EQ(mb->text().toStdString(), "Please fill all the fields");
                mb->close();
            }
        }
    });

    QTest::mouseClick(decryptButton, Qt::LeftButton);
    EXPECT_TRUE(messageBoxShown);

    filePathLineEdit->setText("/some/path");
    keyPathLineEdit->setText("");

    QTimer::singleShot(100, [&]() {
        QWidgetList widgets = QApplication::topLevelWidgets();
        for (QWidget* widget : widgets) {
            if (QMessageBox* mb = qobject_cast<QMessageBox*>(widget)) {
                messageBoxShown = true;
                EXPECT_EQ(mb->text().toStdString(), "Please fill all the fields");
                mb->close();
            }
        }
    });

    QTest::mouseClick(decryptButton, Qt::LeftButton);
    EXPECT_TRUE(messageBoxShown);
}

TEST_F(DecryptDialogTest, DecryptNameDefaultOff) {
    QCheckBox* decryptNameCheckBox = dialog->findChild<QCheckBox*>("decryptNameCheckBox");
    EXPECT_FALSE(decryptNameCheckBox->isChecked());
}
