#include "utils/GuardedContainer.h"
#include "utils/Token.h"
#include <gtest/gtest.h>

class Dummy {
public:
    int value = 0;
};

class GuardedContainerTest : public ::testing::Test {
protected:
};

TEST_F(GuardedContainerTest, AddObjectsAndInvalidateOne) {
    CToken token1, token2;
    CGuardedContainer<Dummy> container;
    Dummy obj1, obj2;
    container.Add(obj1, token1);
    container.Add(obj2, token2);

    int count = 0;
    for (auto& obj : container) {
        count++;
    }
    EXPECT_EQ(count, 2);
    token2.Reset();
    count = 0;
    for (auto& obj : container) {
        count++;
    }
    EXPECT_EQ(count, 1);
}

TEST_F(GuardedContainerTest, AddObjectsAndInvalidateContainer) {
    CToken token1, token2;
    CGuardedContainer<Dummy> container;
    Dummy obj1, obj2;
    container.Add(obj1, token1);
    container.Add(obj2, token2);

    container.Clear();
}