#include <gtest/gtest.h>
#include <QTest>

#include "windows/share_code_dialog.h"

class ShareDialogTest : public ::testing::Test {
protected:
    void SetUp() override {}

};

// === TEST CASES ===
// PopulatesLabels
// CopyButtomEmits
// CopyButtonCopies
// CopyButtoniconChanges
// RefreshButtonEmits
// AutoRefreshes

TEST_F(ShareDialogTest, SanityTest) {
    EXPECT_TRUE(true);
}