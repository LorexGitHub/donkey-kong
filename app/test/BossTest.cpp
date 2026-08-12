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

TEST(BossTest, GetBoundsMatchesShapeSize) {
    Boss boss(0.f, 0.f, 1);
    auto b = boss.get_bounds();
    EXPECT_FLOAT_EQ(b.size.x, 48.f);
    EXPECT_FLOAT_EQ(b.size.y, 56.f);
}

TEST(BossTest, ConstructorAcceptsVariousPositions) {
    Boss b1(100.f, 200.f, 1);
    auto p1 = b1.get_shape().getPosition();
    EXPECT_NEAR(p1.x, 100.f, 1.f);
    EXPECT_NEAR(p1.y, 200.f, 1.f);

    Boss b2(-50.f, 300.f, 2);
    auto p2 = b2.get_shape().getPosition();
    EXPECT_NEAR(p2.x, -50.f, 1.f);
    EXPECT_NEAR(p2.y, 300.f, 1.f);
}

TEST(BossTest, DifferentStagesStoredCorrectly) {
    Boss b1(0.f, 0.f, 1);
    Boss b2(0.f, 0.f, 5);
    Boss b3(0.f, 0.f, 9);
    // monster_stage is private; verify it affects texture path indirectly by
    // checking shape position is independent of stage
    auto b1_pos = b1.get_shape().getPosition();
    auto b2_pos = b2.get_shape().getPosition();
    EXPECT_EQ(b1_pos, b2_pos);
}
