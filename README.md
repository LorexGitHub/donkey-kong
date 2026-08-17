# Ladder Climber

A C++ Donkey Kong-style platformer remake with 9 stages, 8 unique bosses, procedural level generation, and a music system. Built with SFML 3.0 following the Model-View-Controller pattern.

## Demo


https://github.com/user-attachments/assets/2963be98-4ee4-4741-a178-9f643afc5b24



## Gameplay

- Climb 9 stages by navigating platforms and ladders while dodging barrels
- Each stage has a unique boss sprite
- Collect a crown at the top of each stage
- Stage 1: 2 platforms, Stage 2: 4 platforms, Stage 3+: 6 platforms
- Holes appear in platforms from Stage 4 onward
- 3 lives per run — die on a stage to retry; lose all lives for game over
- Stage complete screen shows time vs record — choose NEXT STAGE or RESET (retry)
- After 9 crowns, the overall time is compared to your best — choose PLAY AGAIN

## Controls

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

**Model** (`src/model/`)

- `Player` — position, velocity, jump/climb state, animation
- `Platform` — segmented platforms with destructible holes; `Ladder` struct
- `Barrel` — rolling obstacle with platform collision
- `Boss` — boss character with per-stage sprite
- `GameState` — stage, crowns, lives, timers, records, phase enum

**View** (`src/view/`)

- `GameView` — owns the `sf::RenderWindow`, fonts, textures, and all `draw()` logic

**Control** (`src/control/`)

- `GameController` — processes keyboard/mouse input, modifies model via callbacks

**Coordinator** (`src/Game.hpp/.cpp`)

- `Game` — owns Model objects, View, Controller; runs the game loop and core update logic

## Building

### Prerequisites

- CMake ≥ 3.22
- C++20 compiler (GCC, Clang, or MSVC)
- SFML 3.0 + GoogleTest (downloaded automatically by CMake)

### System dependencies (Linux/WSL)

```bash
sudo apt install build-essential cmake \
  libx11-dev libxrandr-dev libxcursor-dev libxi-dev \
  libgl1-mesa-dev libglu1-mesa-dev \
  libudev-dev libfreetype-dev libvorbis-dev libogg-dev libflac-dev
```

### Build & run

```bash
cd app
mkdir build && cd build
cmake ..
cmake --build . -j4
LD_LIBRARY_PATH=bin ./bin/LadderClimber
```

### Tests

```bash
cd app/build
cmake --build . -j4
LD_LIBRARY_PATH=bin ./bin/LadderClimber_test
```

## JupyterHub (Uni Münster)

The project builds and runs on the university JupyterHub. Because a JupyterHub
session has no physical display, SFML runs headlessly against **Xvfb** (a
virtual X server); the game renders and plays normally inside it.

Run everything (dependencies → build → tests → headless smoke run) with:

```bash
./jupyterhub/run.sh
```

### Step by step (open a Terminal in JupyterLab)

**1. Install dependencies** (once per session):

```bash
sudo apt-get update && sudo apt-get install -y \
    build-essential cmake git \
    libx11-dev libxrandr-dev libxcursor-dev libxi-dev \
    libgl1-mesa-dev libglu1-mesa-dev libudev-dev \
    libfreetype-dev libvorbis-dev libogg-dev libflac-dev \
    xvfb fonts-dejavu-core
```

**2. Build** — CMake downloads SFML 3.0 and GoogleTest on the first run
(network access is required):

```bash
cd app
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4
```

**3. Run the unit tests** (all 60 tests must pass):

```bash
cd app
xvfb-run -a ./build/bin/LadderClimber_test
```

**4. Run the game** headless for 10 seconds as a smoke test (an exit code of
`124` is expected — the timeout killed it, i.e. it started and rendered fine):

```bash
cd app
timeout 10 xvfb-run -a -s "-screen 0 1024x768x24" ./build/bin/LadderClimber
```

### Verification on the JupyterHub

This exact sequence (`./jupyterhub/run.sh`) must complete with
`All JupyterHub checks passed`. It is the same command the course staff can use
to confirm the game builds and runs on the hub.

## File structure

```
ladder-climber/
├── app/
│   ├── CMakeLists.txt
│   ├── src/
│   │   ├── main.cpp
│   │   ├── Game.hpp / Game.cpp
│   │   ├── model/
│   │   │   ├── GameState.hpp / GameState.cpp
│   │   │   ├── Player.hpp / Player.cpp
│   │   │   ├── Platform.hpp / Platform.cpp
│   │   │   ├── Barrel.hpp / Barrel.cpp
│   │   │   ├── Boss.hpp / Boss.cpp
│   │   │   ├── PillarEnemy.hpp / PillarEnemy.cpp
│   │   │   ├── PowerUp.hpp / PowerUp.cpp
│   │   │   └── HeartPickup.hpp / HeartPickup.cpp
│   │   ├── view/
│   │   │   └── GameView.hpp / GameView.cpp
│   │   ├── control/
│   │   │   └── GameController.hpp / GameController.cpp
│   ├── test/
│   │   ├── GameStateTest.cpp
│   │   ├── PlayerTest.cpp
│   │   ├── BarrelTest.cpp
│   │   ├── PowerUpTest.cpp
│   │   ├── PlatformTest.cpp
│   │   ├── BossTest.cpp
│   │   ├── GameControllerTest.cpp
│   │   └── GameViewTest.cpp
│   └── assets/
│       ├── sprites/    # generated .png files
│       ├── music/      # .mp3 files
│       └── fonts/      # arial.ttf (copied from system)
├── jupyterhub/
│   └── run.sh          # deps + build + tests + headless smoke run
└── README.md
```

## Docker

### Build the image

```bash
docker build -t ladder-climber .
```

### Run the game (Linux — native, not WSL)

```bash
docker run --rm \
    -e DISPLAY=$DISPLAY \
    -e PULSE_SERVER=unix:/run/user/1000/pulse/native \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -v /run/user/$(id -u)/pulse:/run/user/1000/pulse \
    --network host \
    ladder-climber
```

### Run the game (WSLg)

> **Prerequisite:** Enable Docker Desktop WSL 2 integration for your distro
> (Settings → Resources → WSL Integration). If `docker` is not found, use `docker.exe` instead.

```bash
docker run --rm \
    -e DISPLAY=$DISPLAY \
    -e PULSE_SERVER=unix:/mnt/wslg/PulseServer \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -v /mnt/wslg:/mnt/wslg \
    --network host \
    ladder-climber
```

### Run the game (Windows — VcXsrv or X410)

First install and launch [VcXsrv](https://sourceforge.net/projects/vcxsrv/) (free).
In the display settings, check **"Disable access control"**. Then:

```powershell
docker run --rm -e DISPLAY=host.docker.internal:0 ladder-climber
```

### Run tests in Docker

```bash
docker run --rm ladder-climber /bin/bash -c "cd /project/app && xvfb-run ./build/bin/LadderClimber_test"
```

## Dependencies

All libraries pulled automatically by CMake via FetchContent:

- [SFML 3.0](https://github.com/SFML/SFML) — windowing, graphics, audio, input
- [GoogleTest](https://github.com/google/googletest) — unit tests
