1. Prompt = "
You are an expert C++ project scaffolder and technical writer. Produce the exact app/src directory structure (tree) and a concise per-file summary so the output matches this project's current state, but do NOT include any pillar enemy or power-up files.

Constraints / context:
- Language: C++20
- Framework: SFML 3.0
- Build: CMake (>=3.22)
- Architecture: Model-View-Controller with a coordinator Game class
- Output must use the exact file paths and casing under app/src/
- Explicit: do NOT include any of these files or types: PillarEnemy.*, PowerUp.*, HeartPickup.*, or any "power-up base" class.

Required output format:
1) An ASCII code block showing the app/src/ folder tree that contains exactly:
   - main.cpp
   - Game.hpp / Game.cpp
   - control/GameController.hpp / control/GameController.cpp
   - view/GameView.hpp / view/GameView.cpp
   - model/
     - GameState.hpp / GameState.cpp
     - Player.hpp / Player.cpp
     - Platform.hpp / Platform.cpp
     - Barrel.hpp / Barrel.cpp
     - Boss.hpp / Boss.cpp
   - tools/gen_sprites.cpp

2) After the tree, for every file above produce a short block:
   - File path (exact)
   - Responsibility: one short sentence
   - For header files: name the main public class and list three key public methods (signatures or short names)
   - For .cpp files: one short note about implemented logic
   - For main.cpp: include a 5–8 line minimal C++ skeleton that constructs Game and runs the main loop (in a code file block named app/src/main.cpp)

Style: concise, factual, do not invent other files or directories, and do not mention removed files.
"

1. Response = "
app/src/
  main.cpp
  Game.hpp
  Game.cpp
  control/
    GameController.hpp
    GameController.cpp
  view/
    GameView.hpp
    GameView.cpp
  model/
    GameState.hpp
    GameState.cpp
    Player.hpp
    Player.cpp
    Platform.hpp
    Platform.cpp
    Barrel.hpp
    Barrel.cpp
    Boss.hpp
    Boss.cpp
  tools/
    gen_sprites.cpp

app/src/main.cpp Responsibility: Program entry; constructs Game (coordinator) and runs the game loop. Minimal skeleton (example):

#include "Game.hpp"
int main(int argc, char** argv) {
    Game game;
    if (!game.init()) return 1;
    game.run();             // blocking main loop: process input, update, render
    return 0;
}

app/src/Game.hpp Responsibility: Coordinator that owns Model, View, Controller and drives the main loop. Public API (main class): - class Game - bool init(); // load assets, init view/controller/model - void run(); // main loop - void update(float dt); // advance logic, called from run()

app/src/Game.cpp Note: Implements Game::init, Game::run (main loop: input -> update -> render), manages ownership of model/view/controller and high-level state transitions.

app/src/control/GameController.hpp Responsibility: Handle user input and translate it into model commands or callbacks. Public API (main class): - class GameController - void processEvent(const sf::Event& ev); // map events to actions - void update(float dt); // per-frame controller updates - void setInputCallbacks(...); // register callbacks into model

app/src/control/GameController.cpp Note: Implements keyboard/gamepad handling, debouncing, debug keys (skip stage, mute), and invokes model callbacks to move/jump/climb.

app/src/view/GameView.hpp Responsibility: Owns sf::RenderWindow, textures, fonts and contains all draw logic. Public API (main class): - class GameView - bool loadAssets(); // load textures/fonts/music - void render(const GameState& state); // draw scene from model state - sf::RenderWindow& getWindow(); // access to the window

app/src/view/GameView.cpp Note: Implements SFML setup, frame clearing, draw order for platforms/barrels/player/boss/UI, and manages texture atlases and animations.

app/src/model/GameState.hpp Responsibility: Holds global run state (stage, crowns, lives, timers, phase enum) and high-level game rules. Public API (main class): - class GameState - void resetStage(int stageNumber); - void advanceStage(); - void update(float dt);

app/src/model/GameState.cpp Note: Implements phase transitions (playing, paused, stage complete), timers/records, serialization of records and crown counts.

app/src/model/Player.hpp Responsibility: Player domain model: position, velocity, jump/climb state, collisions and animation state. Public API (main class): - class Player - void update(float dt, const GameState& state); - void jump(); - void applyDamage(int amount);

app/src/model/Player.cpp Note: Implements physics integration (velocity, gravity), ladder climb logic, collision response with platforms and enemies.

app/src/model/Platform.hpp Responsibility: Represent segmented platforms and ladders positions; manage destructible holes. Public API (main class/struct): - struct Platform - bool collides(const sf::FloatRect& box) const; - std::vector<Segment> segments() const; - void breakAt(int segmentIndex);

app/src/model/Platform.cpp Note: Implements platform segmentation, hole insertion and platform collision helpers used by Player and Barrels.

app/src/model/Barrel.hpp Responsibility: Rolling obstacle model with platform collision/edge detection. Public API (main class): - class Barrel - void update(float dt); - void bounce(); - bool isActive() const;

app/src/model/Barrel.cpp Note: Implements rolling behavior, collisions with platform segments, directional changes at edges and interactions with player.

app/src/model/Boss.hpp Responsibility: Boss entity per-stage, contains sprite id and boss-specific behaviors. Public API (main class): - class Boss - void update(float dt); - void triggerAttackPattern(int patternId); - sf::IntRect spriteRect() const;

app/src/model/Boss.cpp Note: Implements per-stage boss logic and attack timing; exposes collision shapes for player interactions.

app/src/tools/gen_sprites.cpp Responsibility: Offline tool to generate sprite PNGs used in assets/sprites/ (placeholder generator for development). Note: Generates images and writes them to app/assets/sprites/; not part of runtime.
"

