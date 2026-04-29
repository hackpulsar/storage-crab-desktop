#include <gtest/gtest.h>
#include <QTest>
#include <QLabel>
#include <QPushButton>
#include <QClipboard>
#include <QSignalSpy>
#include <QTimer>
#include <QMessageBox>
#include <QPointer>

#include "windows/share_code_dialog.h"

class ShareCodeDialogTest : public ::testing::Test {
protected:
    void SetUp() override {
        dialog = new ShareCodeDialog("randomCode", 67);
        dialog->setAttribute(Qt::WA_DeleteOnClose);

        codeLabel = dialog->findChild<QLabel*>("codeLabel");
        timeLabel = dialog->findChild<QLabel*>("timeLabel");
        copyButton = dialog->findChild<QPushButton*>("copyButton");
        refreshButton = dialog->findChild<QPushButton*>("refreshButton");
        closeButton = dialog->findChild<QPushButton*>("closeButton");
    }

    QPointer<ShareCodeDialog> dialog; // Qt handles lifetime here
    QLabel *codeLabel, *timeLabel;
    QPushButton *copyButton, *refreshButton, *closeButton;

};

TEST_F(ShareCodeDialogTest, HasControls) {
    QLabel* title = dialog->findChild<QLabel*>("accessCodeLabel");
    
    EXPECT_NE(title, nullptr);
    EXPECT_NE(codeLabel, nullptr);
    EXPECT_NE(timeLabel, nullptr);
    EXPECT_NE(copyButton, nullptr);
    EXPECT_NE(refreshButton, nullptr);
    EXPECT_NE(closeButton, nullptr);
}

TEST_F(ShareCodeDialogTest, PopulatesLabels) {
    EXPECT_EQ(codeLabel->text().toStdString(), "randomCode");
    EXPECT_FALSE(timeLabel->text().isEmpty());
}

TEST_F(ShareCodeDialogTest, CopyButtonCopies) {
    QTest::mouseClick(copyButton, Qt::LeftButton);
    EXPECT_EQ(QGuiApplication::clipboard()->text().toStdString(), "randomCode");
}

TEST_F(ShareCodeDialogTest, CopyButtonIconChangesAndResets) {
    QImage iconBefore = copyButton->icon().pixmap(32, 32).toImage();

    QTest::mouseClick(copyButton, Qt::LeftButton);
    QTest::qWait(100); // wait for event to process

    QImage iconAfter = copyButton->icon().pixmap(32, 32).toImage();

    EXPECT_NE(iconBefore, iconAfter);
    EXPECT_FALSE(copyButton->isEnabled());

    QTest::qWait(3100); // waiting for icon to change back

    EXPECT_EQ(copyButton->icon().pixmap(32, 32).toImage(), iconBefore);
    EXPECT_TRUE(copyButton->isEnabled());
}

TEST_F(ShareCodeDialogTest, CloseButtonCloses) {
    QTest::mouseClick(closeButton, Qt::LeftButton);
    QTest::qWait(100); // wait for event to process
    EXPECT_TRUE(dialog.isNull());
}

TEST_F(ShareCodeDialogTest, TimeGoesDown) {
    QString timeBefore = timeLabel->text();
    QTest::qWait(1100); // 1.1 second sleep
    EXPECT_NE(timeBefore, timeLabel->text());
}
