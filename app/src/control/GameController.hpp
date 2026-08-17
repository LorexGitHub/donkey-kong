#ifndef DK_GAMECONTROLLER_H
#define DK_GAMECONTROLLER_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <functional>
#include <vector>

class GameView;
class GameState;
class Player;
struct Ladder;

class GameController {
public:
    GameController(GameView& view, GameState& state, Player& player,
                   std::vector<Ladder>& ladders,
                   sf::Music& music,
                   std::function<void()> on_start_game,
                   std::function<void()> on_play_music,
                   std::function<void()> on_title);

    /// Polls window events and the keyboard, then forwards the semantic
    /// key states to handle_keys() and click positions to handle_mouse_click().
    void handle_input(float dt);

    /// Maps abstract key states to model actions (movement, jump, climb,
    /// pause, stage skip). Pure enough to be driven directly by tests.
    void handle_keys(float dt,
                     bool key_up, bool key_down,
                     bool key_left, bool key_right,
                     bool key_jump, bool key_pause,
                     bool key_prev_stage, bool key_next_stage);

    /// Routes a world-space mouse click to the correct screen action.
    /// Testable without a window: pass world coordinates directly.
    void handle_mouse_click(sf::Vector2f wp);

private:
    GameView& view;
    GameState& state;
    Player& player;
    std::vector<Ladder>& ladders;
    sf::Music& music;
    std::function<void()> start_game_fn;
    std::function<void()> play_music_fn;
    std::function<void()> title_fn;
    float pause_cd = 0;
    bool jump_key_held = false;
};

#endif
