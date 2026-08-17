#ifndef DK_GAME_H
#define DK_GAME_H

#include <SFML/Audio.hpp>
#include <vector>
#include <memory>
#include <string>

#include "model/GameState.hpp"
#include "model/Player.hpp"
#include "model/Platform.hpp"
#include "model/Barrel.hpp"
#include "model/Boss.hpp"
#include "model/PowerUp.hpp"
#include "model/Pickup.hpp"
#include "model/PillarEnemy.hpp"
#include "model/HeartPickup.hpp"
#include "view/GameView.hpp"
#include "control/GameController.hpp"

/// Top-level coordinator: owns Model objects (Player, platforms, barrels, etc.),
/// a GameState, a GameView for rendering, and a GameController for input.
/// Runs the game loop and implements the core game logic in update().
class Game {
public:
    Game();
    void run();

private:
    void update(float dt);
    void start_game();
    void go_to_title();
    void setup_stage();
    void spawn_barrel();
    void spawn_pickups();
    void place_god_powerup(float px);
    void lose_life();
    void check_collisions();
    void play_random_music();

    GameState state;
    GameView view;
    Player player;
    Boss dk{60.f, 140.f, 1};
    std::vector<Platform> platforms;
    std::vector<Ladder> ladders;
    std::vector<std::unique_ptr<Barrel>> barrels;
    std::unique_ptr<Pickup> powerup;
    std::unique_ptr<Pickup> heart_pickup;
    std::unique_ptr<PillarEnemy> left_pillar;
    std::unique_ptr<PillarEnemy> right_pillar;
    float barrel_timer = 0;
    float lava_anim = 0;
    sf::Music music;
    GameController controller;
};

#endif
