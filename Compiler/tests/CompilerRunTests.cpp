
#include <cmath>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::Return;

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}