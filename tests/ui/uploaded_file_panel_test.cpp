#include <gtest/gtest.h>

#include <QTest>
#include <QSignalSpy>
#include <QLabel>
#include <QPushButton>

#include "widgets/uploaded_file_panel.h"
#include "api/file_data.hpp"

class UploadedFilePanelTest : public ::testing::Test {
protected:
    void SetUp() override {
        fileData = {
            .name = "test_file.txt",
            .path = "/remote/storage/path/test_file.txt",
            .size = 1500000,
            .id = 42
        };
        panel = std::make_unique<UploadedFilePanel>(fileData);
    }

    FileData fileData;
    std::unique_ptr<UploadedFilePanel> panel;
};

TEST_F(UploadedFilePanelTest, HasButtons) {
    QPushButton* shareButton = panel->findChild<QPushButton*>("shareButton");
    QPushButton* downloadButton = panel->findChild<QPushButton*>("downloadButton");
    QPushButton* deleteButton = panel->findChild<QPushButton*>("deleteButton");

    ASSERT_NE(shareButton, nullptr);
    ASSERT_NE(downloadButton, nullptr);
    ASSERT_NE(deleteButton, nullptr);
}

TEST_F(UploadedFilePanelTest, PopulatesLabelsFromFileData) {
    panel->resize(2000, 77); // Resize to avoid elision.

    QLabel* filenameLabel = panel->findChild<QLabel*>("filenameLabel");
    QLabel* pathLabel = panel->findChild<QLabel*>("pathLabel");
    QLabel* sizeLabel = panel->findChild<QLabel*>("sizeLabel");

    EXPECT_EQ(filenameLabel->text().toStdString(), fileData.name);
    EXPECT_EQ(pathLabel->text().toStdString(), fileData.path);
    EXPECT_EQ(sizeLabel->text().toStdString(), std::to_string(fileData.size / (1000.f * 1000.f)) + "mb");
}

TEST_F(UploadedFilePanelTest, ShareButtonEmits) {
    QPushButton* shareButton = panel->findChild<QPushButton*>("shareButton");
    ASSERT_NE(shareButton, nullptr);

    QSignalSpy spy(panel.get(), &UploadedFilePanel::shareButtonPressed);
    QTest::mouseClick(shareButton, Qt::LeftButton);

    ASSERT_EQ(spy.size(), 1);
    EXPECT_EQ(spy.at(0).at(0).toUInt(), fileData.id);
}

TEST_F(UploadedFilePanelTest, DownloadButtonEmits) {
    QPushButton* downloadButton = panel->findChild<QPushButton*>("downloadButton");
    ASSERT_NE(downloadButton, nullptr);

    qRegisterMetaType<FileData>();
    QSignalSpy spy(panel.get(), &UploadedFilePanel::downloadButtonPressed);
    QTest::mouseClick(downloadButton, Qt::LeftButton);

    FileData received = qvariant_cast<FileData>(spy.at(0).at(0));

    ASSERT_EQ(spy.size(), 1);

    ASSERT_EQ(fileData.name, received.name);
    ASSERT_EQ(fileData.path, received.path);
    ASSERT_EQ(fileData.size, received.size);
    ASSERT_EQ(fileData.id, received.id);
}

TEST_F(UploadedFilePanelTest, DeleteButtonEmits) {
    QPushButton* deleteButton = panel->findChild<QPushButton*>("deleteButton");
    ASSERT_NE(deleteButton, nullptr);

    QSignalSpy spy(panel.get(), &UploadedFilePanel::deleteButtonPressed);
    QTest::mouseClick(deleteButton, Qt::LeftButton);

    ASSERT_EQ(spy.size(), 1);
    EXPECT_EQ(spy.at(0).at(0).toUInt(), fileData.id);
}

TEST_F(UploadedFilePanelTest, ElidesLongFilename) {
    FileData longNameData = {
        .name = "this_is_a_very_long_filename_that_should_be_elided.txt",
        .path = "/short",
        .size = 100,
        .id = 1
    };
    UploadedFilePanel longPanel(longNameData);

    QLabel* pathLabel = longPanel.findChild<QLabel*>("pathLabel");
    QString result = panel->elideText(longNameData.name.c_str(), pathLabel->font(), 50);

    EXPECT_NE(result, longNameData.name);
    EXPECT_TRUE(result.contains("…"));
}

TEST_F(UploadedFilePanelTest, ElidesLongPath) {
    FileData longPathData = {
        .name = "file.txt",
        .path = "/very/deep/nested/remote/storage/path/that/should/be/elided",
        .size = 100,
        .id = 1
    };
    UploadedFilePanel longPanel(longPathData);

    QLabel* pathLabel = longPanel.findChild<QLabel*>("pathLabel");
    QString result = panel->elideText(longPathData.path.c_str(), pathLabel->font(), 50);

    EXPECT_TRUE(result.contains("…"));
}

TEST_F(UploadedFilePanelTest, LargeWidthNoElision) {
    FileData longNameData = {
        .name = "this_is_a_very_long_filename_that_should_be_elided.txt",
        .path = "/short",
        .size = 100,
        .id = 1
    };
    UploadedFilePanel longPanel(longNameData);
    QLabel* filenameLabel = longPanel.findChild<QLabel*>("filenameLabel");
    QString result = panel->elideText(longNameData.name.c_str(), filenameLabel->font(), 2000);

    EXPECT_FALSE(result.contains("…"));
}
