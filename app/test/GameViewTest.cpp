#include <gtest/gtest.h>
#include "view/GameView.hpp"
#include "model/GameState.hpp"
#include "model/Player.hpp"
#include "model/Platform.hpp"
#include "model/Barrel.hpp"
#include "model/Boss.hpp"
#include "model/PillarEnemy.hpp"
#include <memory>

namespace {

// One shared window for all view tests (needs a display; use xvfb-run headless).
GameView g_view;

bool inside_window(const sf::FloatRect& r) {
    return r.position.x >= 0.f && r.position.y >= 0.f &&
           r.position.x + r.size.x <= 800.f &&
           r.position.y + r.size.y <= 750.f;
}

} // namespace

// ── Static layout helpers (testable without rendering) ──

TEST(GameViewTest, TitleButtonsInsideWindow) {
    EXPECT_TRUE(inside_window(GameView::title_btn_easy()));
    EXPECT_TRUE(inside_window(GameView::title_btn_normal()));
    EXPECT_TRUE(inside_window(GameView::title_btn_hard()));
    EXPECT_TRUE(inside_window(GameView::title_btn_custom()));
    EXPECT_TRUE(inside_window(GameView::title_btn_play()));
}

TEST(GameViewTest, TitleButtonsHaveSize) {
    EXPECT_GT(GameView::title_btn_easy().size.x, 0.f);
    EXPECT_GT(GameView::title_btn_easy().size.y, 0.f);
    EXPECT_GT(GameView::title_btn_play().size.x, 0.f);
}

TEST(GameViewTest, DifficultyButtonsDoNotOverlap) {
    auto easy = GameView::title_btn_easy();
    auto normal = GameView::title_btn_normal();
    auto hard = GameView::title_btn_hard();
    EXPECT_FALSE(easy.findIntersection(normal).has_value());
    EXPECT_FALSE(normal.findIntersection(hard).has_value());
    EXPECT_FALSE(easy.findIntersection(hard).has_value());
}

TEST(GameViewTest, CustomSlidersInsideWindow) {
    EXPECT_TRUE(inside_window(GameView::custom_speed_track()));
    EXPECT_TRUE(inside_window(GameView::custom_interval_track()));
    EXPECT_TRUE(inside_window(GameView::custom_pillar_speed_track()));
    EXPECT_TRUE(inside_window(GameView::custom_fire_rate_track()));
    EXPECT_TRUE(inside_window(GameView::custom_fire_speed_track()));
    EXPECT_TRUE(inside_window(GameView::custom_btn_play()));
}

// ── Instance state helpers ──

TEST(GameViewTest, MenuButtonBoundsNonEmpty) {
    EXPECT_GT(g_view.get_menu_btn_bounds().size.x, 0.f);
    EXPECT_GT(g_view.get_pause_resume_btn_bounds().size.x, 0.f);
    EXPECT_GT(g_view.get_pause_reset_btn_bounds().size.x, 0.f);
    EXPECT_GT(g_view.get_mute_btn_bounds().size.x, 0.f);
}

TEST(GameViewTest, MuteButtonLocatedInCorner) {
    auto m = g_view.get_mute_btn_bounds();
    EXPECT_LT(m.position.x, 40.f);
    EXPECT_LT(m.position.y, 40.f);
}

// ── Rendering smoke test ──

TEST(GameViewTest, DrawPlayingSceneRenders) {
    GameState state;
    state.phase = GameState::Phase::Playing;
    state.stage = 1;
    state.lives = 3;

    Player player;
    player.set_position(100, 710);

    std::vector<Platform> platforms;
    platforms.emplace_back(50.f, 140.f, 700.f);
    platforms.emplace_back(50.f, 710.f, 700.f);

    std::vector<Ladder> ladders;
    ladders.emplace_back(680.f, 140.f, 710.f);

    std::vector<std::unique_ptr<Barrel>> barrels;
    barrels.push_back(std::make_unique<Barrel>(60.f, 140.f, 180.f, 0));

    Boss dk(60.f, 140.f, 1);
    PillarEnemy left(35.f, 140.f, 710.f, 1);
    PillarEnemy right(765.f, 140.f, 710.f, -1);

    EXPECT_NO_THROW(g_view.draw(state, player, platforms, ladders, barrels, dk,
                                0.f, nullptr, nullptr, &left, &right));
}

TEST(GameViewTest, DrawWonScreenRenders) {
    GameState state;
    state.phase = GameState::Phase::Won;
    state.stage = 1;
    state.crowns = 1;

    Player player;
    std::vector<Platform> platforms;
    platforms.emplace_back(50.f, 140.f, 700.f);
    std::vector<Ladder> ladders;
    std::vector<std::unique_ptr<Barrel>> barrels;
    Boss dk(60.f, 140.f, 1);
    PillarEnemy left(35.f, 140.f, 710.f, 1);
    PillarEnemy right(765.f, 140.f, 710.f, -1);

    EXPECT_NO_THROW(g_view.draw(state, player, platforms, ladders, barrels, dk,
                                0.f, nullptr, nullptr, &left, &right));
}