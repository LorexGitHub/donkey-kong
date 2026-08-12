#include <gtest/gtest.h>
#include "model/Platform.hpp"

TEST(PlatformTest, StartsFullySolid) {
    Platform p(50, 100, 700);
    for (int i = 0; i < 10; i++)
        EXPECT_TRUE(p.is_solid(i));
}

TEST(PlatformTest, SetSolidCreatesHole) {
    Platform p(50, 100, 700);
    p.set_solid(3, false);
    EXPECT_FALSE(p.is_solid(3));
    EXPECT_TRUE(p.is_solid(4));
}

TEST(PlatformTest, SetSolidTogglesState) {
    Platform p(10.f, 20.f, 500.f);
    p.set_solid(7, false);
    EXPECT_FALSE(p.is_solid(7));
    p.set_solid(7, true);
    EXPECT_TRUE(p.is_solid(7));
}

TEST(PlatformTest, SetSolidOutOfBoundsIgnored) {
    Platform p(0, 0, 100);
    p.set_solid(-1, false);
    p.set_solid(10, false);
    p.set_solid(100, false);
    for (int i = 0; i < 10; i++)
        EXPECT_TRUE(p.is_solid(i));
}

TEST(PlatformTest, BoundsMatchParameters) {
    Platform p(50, 100, 700);
    auto b = p.get_bounds();
    EXPECT_EQ(b.position.x, 50);
    EXPECT_EQ(b.position.y, 100);
    EXPECT_EQ(b.size.x, 700);
}
