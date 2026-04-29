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

#include "windows/upload_dialog.h"

class UploadDialogTest : public ::testing::Test {
protected:
    void SetUp() override {
        dialog = new UploadDialog("/path/to/a/file");
        dialog->setAttribute(Qt::WA_DeleteOnClose);

        filePathLineEdit = dialog->findChild<QLineEdit*>("filePathLineEdit");
        keyPathLineEdit = dialog->findChild<QLineEdit*>("keyPathLineEdit");
        cancelButton = dialog->findChild<QPushButton*>("cancelButton");
        uploadButton = dialog->findChild<QPushButton*>("uploadButton");
        keySizeComboBox = dialog->findChild<QComboBox*>("keySizeComboBox");
        aesTypeComboBox = dialog->findChild<QComboBox*>("aesTypeComboBox");
        algorithmComboBox = dialog->findChild<QComboBox*>("algorithmComboBox");
    }

    QPointer<UploadDialog> dialog;
    QLineEdit *filePathLineEdit, *keyPathLineEdit;
    QComboBox *keySizeComboBox, *aesTypeComboBox, *algorithmComboBox;
    QPushButton *cancelButton, *uploadButton;

};

TEST_F(UploadDialogTest, HasControls) {
    // Labels
    EXPECT_NE(dialog->findChild<QLabel*>("filePathLabel"), nullptr);
    EXPECT_NE(dialog->findChild<QLabel*>("keyPathLabel"), nullptr);
    EXPECT_NE(dialog->findChild<QLabel*>("algorithmLabel"), nullptr);
    EXPECT_NE(dialog->findChild<QLabel*>("aesTypeLabel"), nullptr);
    EXPECT_NE(dialog->findChild<QLabel*>("keySizeLabel"), nullptr);
    EXPECT_NE(dialog->findChild<QLabel*>("encryptNameLabel"), nullptr);

    // Fields
    EXPECT_NE(filePathLineEdit, nullptr);
    EXPECT_NE(keyPathLineEdit, nullptr);
    EXPECT_NE(keySizeComboBox, nullptr);
    EXPECT_NE(aesTypeComboBox, nullptr);
    EXPECT_NE(algorithmComboBox, nullptr);
    EXPECT_NE(dialog->findChild<QCheckBox*>("encryptNameCheckBox"), nullptr);

    // Buttons
    EXPECT_NE(dialog->findChild<QPushButton*>("chooseFilePathButton"), nullptr);
    EXPECT_NE(dialog->findChild<QPushButton*>("chooseKeyPathButton"), nullptr);
    EXPECT_NE(uploadButton, nullptr);
    EXPECT_NE(cancelButton, nullptr);
}

TEST_F(UploadDialogTest, PopulatesFilePath) {
    EXPECT_EQ(filePathLineEdit->text(), "/path/to/a/file");
}

TEST_F(UploadDialogTest, AlgorithmTypes) {
    EXPECT_EQ(algorithmComboBox->count(), 2);
    EXPECT_EQ(algorithmComboBox->itemText(0), "AES");
    EXPECT_EQ(algorithmComboBox->itemText(1), "Hybrid (AES + RSA)");
}

TEST_F(UploadDialogTest, KeySizes) {
    EXPECT_EQ(keySizeComboBox->count(), 3);
    EXPECT_EQ(keySizeComboBox->itemText(0), "1024");
    EXPECT_EQ(keySizeComboBox->itemText(1), "2048");
    EXPECT_EQ(keySizeComboBox->itemText(2), "4096");
}

TEST_F(UploadDialogTest, AESTypes) {
    EXPECT_EQ(aesTypeComboBox->count(), 3);
    EXPECT_EQ(aesTypeComboBox->itemText(0), "AES-128");
    EXPECT_EQ(aesTypeComboBox->itemText(1), "AES-192");
    EXPECT_EQ(aesTypeComboBox->itemText(2), "AES-256");
}

TEST_F(UploadDialogTest, CancelCloses) {
    QTest::mouseClick(cancelButton, Qt::LeftButton);
    QTest::qWait(100); // wait for event to process
    EXPECT_TRUE(dialog.isNull());
}

TEST_F(UploadDialogTest, EmptyFieldsError) {
    filePathLineEdit->setText("");
    keyPathLineEdit->setText("/some/path");
    
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

    QTest::mouseClick(uploadButton, Qt::LeftButton);
    EXPECT_TRUE(messageBoxShown);

    filePathLineEdit->setText("/some/path");
    keyPathLineEdit->setText("");

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

    QTest::mouseClick(uploadButton, Qt::LeftButton);
    EXPECT_TRUE(messageBoxShown);
}

TEST_F(UploadDialogTest, EncryptNameCheckBoxDefaultOff) {
    EXPECT_FALSE(dialog->findChild<QCheckBox*>("encryptNameCheckBox")->isChecked());
}
