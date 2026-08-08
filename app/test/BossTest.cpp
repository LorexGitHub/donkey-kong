#include <gtest/gtest.h>
#include "model/Boss.hpp"

TEST(BossTest, ConstructorSetsPosition) {
    Boss boss(60.f, 140.f, 1);
    auto pos = boss.get_shape().getPosition();
    EXPECT_NEAR(pos.x, 60.f, 1.f);
    EXPECT_NEAR(pos.y, 140.f, 1.f);
}

TEST(BossTest, GetBoundsNonEmpty) {
    Boss boss(60.f, 140.f, 1);
    auto b = boss.get_bounds();
    EXPECT_GT(b.size.x, 0);
    EXPECT_GT(b.size.y, 0);
}
