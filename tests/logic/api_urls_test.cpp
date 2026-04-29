#include <gtest/gtest.h>
#include <cstdlib>

#include "api/api.h"

class ApiUrlsTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        setenv("DOWNLOAD_URL", "/api/files/download/", 1);
        setenv("DELETE_URL", "/api/files/delete/", 1);
        setenv("SHARE_URL", "/api/files/share/", 1);
        setenv("DOWNLOAD_SHARED_URL", "/api/files/download/shared/", 1);
    }
};

TEST_F(ApiUrlsTest, UrlBuildersReturnValidUrls) {
    constexpr size_t testFileId = 123;

    std::string downloadUrl = API::DOWNLOAD_URL_FOR(testFileId);
    std::string deleteUrl = API::DELETE_URL_FOR(testFileId);
    std::string shareUrl = API::SHARE_URL_FOR(testFileId);
    std::string sharedUrl = API::DOWNLOAD_SHARED_URL_FOR("code123");

    EXPECT_EQ(downloadUrl, "/api/files/download/123/");
    EXPECT_EQ(deleteUrl, "/api/files/delete/123/");
    EXPECT_EQ(shareUrl, "/api/files/share/123/");
    EXPECT_EQ(sharedUrl, "/api/files/download/shared/code123/");
}