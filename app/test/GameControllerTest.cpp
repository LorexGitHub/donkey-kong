#include <gtest/gtest.h>
#include "control/GameController.hpp"
#include "view/GameView.hpp"
#include "model/GameState.hpp"
#include "model/Player.hpp"
#include "model/Platform.hpp"
#include <SFML/Audio.hpp>
#include <vector>

namespace {

// One shared window for all controller tests (avoids opening a window
// per test). Requires a display: run headless with xvfb-run.
GameView g_view;

/// Fixture wiring a GameController to a fresh GameState/Player/Ladder set
/// and recording which callbacks fired.
struct Harness {
    GameState state;
    Player player;
    std::vector<Ladder> ladders;
#if LC_AUDIO
    sf::Music music;
#endif
    bool started = false;
    bool to_title = false;
    bool music_played = false;
    GameController controller;

    Harness()
        : controller(g_view, state, player, ladders,
#if LC_AUDIO
                     music,
#endif
                     [this]() { started = true; },
                     [this]() { music_played = true; },
                     [this]() { to_title = true; }) {}
};

sf::Vector2f center(const sf::FloatRect& r) {
    return {r.position.x + r.size.x / 2.f, r.position.y + r.size.y / 2.f};
}

} // namespace

// ── handle_keys: movement ──

TEST(GameControllerTest, LeftKeyMovesPlayerLeft) {
    Harness h;
    h.state.phase = GameState::Phase::Playing;
    h.player.set_position(400, 200);
    h.controller.handle_keys(0.016f, false, false, true, false, false, false, false, false);
    h.player.update(0.016f);
    EXPECT_LT(h.player.get_pos().x, 400);
}

TEST(GameControllerTest, RightKeyMovesPlayerRight) {
    Harness h;
    h.state.phase = GameState::Phase::Playing;
    h.player.set_position(100, 200);
    h.controller.handle_keys(0.016f, false, false, false, true, false, false, false, false);
    h.player.update(0.016f);
    EXPECT_GT(h.player.get_pos().x, 100);
}

TEST(GameControllerTest, JumpKeyMakesPlayerJump) {
    Harness h;
    h.state.phase = GameState::Phase::Playing;
    h.player.set_position(100, 200);
    float y0 = h.player.get_pos().y;
    h.controller.handle_keys(0.016f, false, false, false, false, true, false, false, false);
    h.player.update(0.016f);
    EXPECT_FALSE(h.player.is_on_ground());
    EXPECT_LT(h.player.get_pos().y, y0);
}

TEST(GameControllerTest, JumpKeyEdgeTriggerOnlyOnce) {
    Harness h;
    h.state.phase = GameState::Phase::Playing;
    h.player.set_position(100, 200);
    // Two frames with the jump key held: only the first frame must trigger a jump.
    h.controller.handle_keys(0.016f, false, false, false, false, true, false, false, false);
    int after_first = h.player.get_jumps_left();
    h.controller.handle_keys(0.016f, false, false, false, false, true, false, false, false);
    EXPECT_EQ(h.player.get_jumps_left(), after_first);
}

TEST(GameControllerTest, UpKeyClimbsLadder) {
    Harness h;
    h.state.phase = GameState::Phase::Playing;
    h.ladders.emplace_back(100.f, 100.f, 300.f);
    h.player.set_position(100, 200);
    h.controller.handle_keys(0.016f, true, false, false, false, false, false, false, false);
    EXPECT_TRUE(h.player.is_climbing());
    float y0 = h.player.get_pos().y;
    h.player.update(0.016f);
    EXPECT_LT(h.player.get_pos().y, y0);
}

// ── handle_keys: pause ──

TEST(GameControllerTest, PauseKeyTogglesPause) {
    Harness h;
    h.state.phase = GameState::Phase::Playing;
    h.state.paused = false;
    h.controller.handle_keys(0.016f, false, false, false, false, false, true, false, false);
    EXPECT_TRUE(h.state.paused);
}

TEST(GameControllerTest, PauseCooldownBlocksRapidToggle) {
    Harness h;
    h.state.phase = GameState::Phase::Playing;
    h.controller.handle_keys(0.016f, false, false, false, false, false, true, false, false);
    EXPECT_TRUE(h.state.paused);
    h.controller.handle_keys(0.016f, false, false, false, false, false, true, false, false);
    EXPECT_TRUE(h.state.paused);
}

// ── handle_keys: stage skip / WON advance ──

TEST(GameControllerTest, NextStageKeyIncrementsCrowns) {
    Harness h;
    h.state.phase = GameState::Phase::Playing;
    h.state.crowns = 3;
    h.controller.handle_keys(0.016f, false, false, false, false, false, false, false, true);
    EXPECT_EQ(h.state.crowns, 4);
    EXPECT_TRUE(h.started);
}

TEST(GameControllerTest, PrevStageKeyDecrementsCrowns) {
    Harness h;
    h.state.phase = GameState::Phase::Playing;
    h.state.crowns = 3;
    h.controller.handle_keys(0.016f, false, false, false, false, false, false, true, false);
    EXPECT_EQ(h.state.crowns, 2);
    EXPECT_TRUE(h.started);
}

TEST(GameControllerTest, WonScreenSpaceAdvancesStage) {
    Harness h;
    h.state.phase = GameState::Phase::Won;
    h.state.crowns = 2;
    h.controller.handle_keys(0.016f, false, false, false, false, true, false, false, false);
    EXPECT_TRUE(h.started);
    EXPECT_EQ(h.state.lives, 3);
}

// ── handle_mouse_click ──

TEST(GameControllerTest, TitleClickSelectsDifficulty) {
    Harness h;
    h.state.phase = GameState::Phase::Title;
    h.controller.handle_mouse_click(center(GameView::title_btn_hard()));
    EXPECT_EQ(h.state.difficulty, GameState::Difficulty::Hard);
}

TEST(GameControllerTest, TitleClickStartsGame) {
    Harness h;
    h.state.phase = GameState::Phase::Title;
    h.controller.handle_mouse_click(center(GameView::title_btn_play()));
    EXPECT_TRUE(h.started);
    EXPECT_TRUE(h.to_title);
}

TEST(GameControllerTest, CustomClickOpensCustomScreen) {
    Harness h;
    h.state.phase = GameState::Phase::Title;
    h.controller.handle_mouse_click(center(GameView::title_btn_custom()));
    EXPECT_EQ(h.state.phase, GameState::Phase::Custom);
    EXPECT_EQ(h.state.difficulty, GameState::Difficulty::Custom);
}

TEST(GameControllerTest, CustomSliderAdjustsBarrelSpeed) {
    Harness h;
    h.state.phase = GameState::Phase::Custom;
    h.controller.handle_mouse_click(center(GameView::custom_speed_track()));
    // Track spans 250..550 px; center maps to the middle of the range 60..400.
    EXPECT_NEAR(h.state.custom_speed, 230.f, 1.f);
}

TEST(GameControllerTest, MuteClickTogglesMute) {
    Harness h;
    h.state.muted = false;
    h.controller.handle_mouse_click(center(g_view.get_mute_btn_bounds()));
    EXPECT_TRUE(h.state.muted);
}

TEST(GameControllerTest, PauseResumeClickUnpauses) {
    Harness h;
    h.state.phase = GameState::Phase::Playing;
    h.state.paused = true;
    h.controller.handle_mouse_click(center(g_view.get_pause_resume_btn_bounds()));
    EXPECT_FALSE(h.state.paused);
}

TEST(GameControllerTest, GameOverMenuReturnsToTitle) {
    Harness h;
    h.state.phase = GameState::Phase::GameOver;
    h.controller.handle_mouse_click(center(g_view.get_menu_btn_bounds()));
    EXPECT_TRUE(h.to_title);
    EXPECT_TRUE(h.music_played);
    EXPECT_EQ(h.state.lives, 3);
}

TEST(GameControllerTest, WonPlayAgainStartsNewRun) {
    Harness h;
    h.state.phase = GameState::Phase::Won;
    h.state.crowns = 9;
    h.controller.handle_mouse_click(center(g_view.get_menu_btn_bounds()));
    EXPECT_TRUE(h.started);
    EXPECT_TRUE(h.to_title);
    EXPECT_EQ(h.state.overall_timer, 0.f);
}