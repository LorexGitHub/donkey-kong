# Ladder Climber

A C++ Donkey Kong-style platformer: climb 9 stages, dodge barrels, collect the crown
and beat your best time. Built with SFML 3.0 following the Model-View-Controller pattern.

## Demo

https://github.com/user-attachments/assets/2963be98-4ee4-4741-a178-9f643afc5b24

## Run

SFML 3.0 and GoogleTest are downloaded and built automatically by CMake (no manual
installation).

### Option A — Local build

**Linux / WSL:**

```bash
sudo apt install build-essential cmake libx11-dev libxrandr-dev libxcursor-dev \
  libxi-dev libgl1-mesa-dev libglu1-mesa-dev libudev-dev libfreetype-dev \
  libvorbis-dev libogg-dev libflac-dev

cd app
cmake -S . -B build
cmake --build build -j4
./build/bin/LadderClimber
```

**Windows** (Visual Studio 2022 with the C++ workload, plus CMake):

```powershell
cd app
cmake -S . -B build
cmake --build build --config Release
.\build\Release\LadderClimber.exe
```

**Tests:**

```bash
# Linux
./build/bin/LadderClimber_test
# Windows
.\build\Release\LadderClimber_test.exe
```

### Option B — Docker

```bash
docker build -t ladder-climber .
```

**Linux** (run from an X11 session):

```bash
docker run --rm -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix --network host ladder-climber
```

**Windows (WSLg)** — needs Docker Desktop with WSL 2 integration enabled:

```bash
docker run --rm -e DISPLAY=$DISPLAY -e PULSE_SERVER=unix:/mnt/wslg/PulseServer \
  -v /tmp/.X11-unix:/tmp/.X11-unix -v /mnt/wslg:/mnt/wslg --network host ladder-climber
```

**Tests (headless):**

```bash
docker run --rm ladder-climber /bin/bash -c "cd /project/app && xvfb-run ./build/bin/LadderClimber_test"
```

### Option C — JupyterHub (Uni Münster)

Build, run the tests and smoke-run the game headlessly (Xvfb) on the course
JupyterHub. No root needed — it uses the prebuilt SFML 3.0 from the course
template. Music is skipped automatically if the hub lacks the audio libraries.

```bash
./jupyterhub/run.sh
```

Run manually from a JupyterLab terminal if you prefer:

```bash
cd app
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4
xvfb-run ./build/bin/LadderClimber_test          # all tests pass
timeout 10 xvfb-run ./build/bin/LadderClimber    # exit code 124 = ran fine
```

The script prints `All JupyterHub checks passed` when everything works.

## How to play

Climb 9 stages: move across the platforms, use the ladders to climb, and dodge the
barrels rolling down. Reach the crown at the top to finish a stage; collect all 9
crowns and finish with the best overall time. You have 3 lives — touching a barrel
or the lava costs one; lose all three and it is game over. A god-mode power-up and
heart pickups occasionally appear to help.

Controls:

| Key           | Action                                |
| ------------- | ------------------------------------- |
| ← → / A D     | Move left/right                       |
| Space / W / ↑ | Jump                                  |
| ↑ / W         | Climb up ladder                       |
| ↓ / S         | Climb down ladder                     |
| Esc / P       | Pause                                 |
| M             | Toggle mute                           |
| U / I         | Skip to previous / next stage (debug) |

## Architecture (MVC)

- **Model** (`app/src/model/`) — game state and rules: `GameState` (stage, crowns,
  lives, timers, records), `Player`, `Platform`, `Ladder`, `Barrel`, `Boss`,
  `PillarEnemy`. `Pickup` is an abstract base class; `PowerUp` and `HeartPickup`
  inherit from it.
- **View** (`app/src/view/`) — `GameView` owns the `sf::RenderWindow` and all drawing.
- **Control** (`app/src/control/`) — `GameController` maps keyboard/mouse input to
  model actions.
- **Coordinator** (`app/src/Game.hpp/.cpp`) — owns the Model, View and Controller,
  runs the game loop and update logic.

## Dependencies

Pulled in automatically by CMake (FetchContent):

- [SFML 3.0](https://github.com/SFML/SFML) — windowing, graphics, audio, input
- [GoogleTest](https://github.com/google/googletest) — unit tests

## File structure

```
ladder-climber/
├── app/
│   ├── CMakeLists.txt
│   ├── src/
│   │   ├── main.cpp
│   │   ├── Game.hpp / Game.cpp
│   │   ├── model/        GameState, Player, Platform, Barrel, Boss, PillarEnemy, PowerUp, HeartPickup
│   │   ├── view/         GameView
│   │   └── control/      GameController
│   ├── test/             GameStateTest, PlayerTest, BarrelTest, PowerUpTest, PlatformTest, BossTest, GameControllerTest, GameViewTest
│   └── assets/           sprites/, music/, fonts/
├── jupyterhub/
│   └── run.sh
└── README.md
```