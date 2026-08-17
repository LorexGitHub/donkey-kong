#!/usr/bin/env bash
# Build, test and smoke-run Ladder Climber on the Uni Münster JupyterHub.
# JupyterHub has no physical display, so everything runs headless via Xvfb.
#
# Usage:  ./jupyterhub/run.sh
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT/app"

echo "==> Installing system dependencies"
sudo apt-get update
sudo apt-get install -y \
    build-essential cmake git \
    libx11-dev libxrandr-dev libxcursor-dev libxi-dev \
    libgl1-mesa-dev libglu1-mesa-dev libudev-dev \
    libfreetype-dev libvorbis-dev libogg-dev libflac-dev \
    xvfb fonts-dejavu-core

echo "==> Building (downloads SFML 3.0 + GoogleTest on first run)"
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"

echo "==> Running unit tests (headless)"
xvfb-run -a ./build/bin/LadderClimber_test

echo "==> Smoke-running the game (10s, headless)"
set +e
timeout 10 xvfb-run -a -s "-screen 0 1024x768x24" ./build/bin/LadderClimber
status=$?
set -e
if [ "$status" -eq 124 ]; then
    echo "==> OK: game ran the full 10 seconds (killed by timeout as expected)"
else
    echo "==> ERROR: game exited early with code $status"
    exit "$status"
fi

echo "==> All JupyterHub checks passed"
