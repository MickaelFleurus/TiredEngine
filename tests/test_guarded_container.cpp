#include "utils/GuardedContainer.h"
#include "utils/Token.h"
#include <gtest/gtest.h>

class Dummy {
public:
    int value = 0;
};

class GuardedContainerTest : public ::testing::Test {
protected:
    CToken token;
    CGuardedContainer<Dummy> container;
    Dummy obj1, obj2;
};

TEST_F(GuardedContainerTest, AddObject) {
    container.Add(obj1, token);
    // TODO: Check that obj1 is in the container
}

TEST_F(GuardedContainerTest, RemoveObject) {
    container.Add(obj1, token);
    container.Remove(obj1);
    // TODO: Check that obj1 is not in the container
}

TEST_F(GuardedContainerTest, IteratorSkipsInvalid) {
    container.Add(obj1, token);
    container.Add(obj2, token);
    // TODO: Invalidate obj1's token and check iterator skips it
}

TEST_F(GuardedContainerTest, MultipleObjects) {
    container.Add(obj1, token);
    container.Add(obj2, token);
    // TODO: Check both objects are present
}
