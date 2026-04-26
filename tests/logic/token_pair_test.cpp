#include <gtest/gtest.h>
#include <thread>
#include <vector>

#include "api/token_pair.h"

class TokenPairTest : public ::testing::Test {
protected:
    API::TokenPair pair;
};

TEST_F(TokenPairTest, EmptyOnConstruction) {
    EXPECT_EQ(pair.getAccess(), "");
    EXPECT_EQ(pair.getRefresh(), "");
}

TEST_F(TokenPairTest, StoresAndRetrievesAccessToken) {
    pair.store("access123", "refresh456");
    EXPECT_EQ(pair.getAccess(), "access123");
}

TEST_F(TokenPairTest, StoresAndRetrievesRefreshToken) {
    pair.store("access123", "refresh456");
    EXPECT_EQ(pair.getRefresh(), "refresh456");
}

TEST_F(TokenPairTest, UpdateAccessToken) {
    pair.store("access1", "refresh1");
    pair.setAccess("new_access");
    EXPECT_EQ(pair.getAccess(), "new_access");
    EXPECT_EQ(pair.getRefresh(), "refresh1");
}

TEST_F(TokenPairTest, UpdateRefreshToken) {
    pair.store("access1", "refresh1");
    pair.setRefresh("new_refresh");
    EXPECT_EQ(pair.getAccess(), "access1");
    EXPECT_EQ(pair.getRefresh(), "new_refresh");
}

TEST_F(TokenPairTest, ConstructorInitializesTokens) {
    API::TokenPair pair("access_ctor", "refresh_ctor");
    EXPECT_EQ(pair.getAccess(), "access_ctor");
    EXPECT_EQ(pair.getRefresh(), "refresh_ctor");
}

TEST_F(TokenPairTest, ConcurrentReadWrite) {
    std::vector<std::thread> threads;
    constexpr int numWriters = 5;
    constexpr int numReaders = 5;

    std::atomic<int> writeCount{0};
    std::atomic<int> readCount{0};

    for (int i = 0; i < numWriters; ++i) {
        threads.emplace_back([this, i, &writeCount]() {
            pair.store("access_" + std::to_string(i), "refresh_" + std::to_string(i));
            writeCount++;
        });
    }

    for (int i = 0; i < numReaders; ++i) {
        threads.emplace_back([this, &readCount]() {
            volatile auto access = pair.getAccess();
            (void)access;
            auto refresh = pair.getRefresh();
            (void)refresh;
            readCount++;
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    EXPECT_EQ(writeCount, numWriters);
    EXPECT_EQ(readCount, numReaders);
}