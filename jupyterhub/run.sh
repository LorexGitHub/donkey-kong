#!/usr/bin/env bash
# Build, test and smoke-run Ladder Climber on the Uni Münster JupyterHub.
# JupyterHub has no physical display, so everything runs headless via Xvfb.
# The hub image already ships the C++ toolchain and Xvfb; no root needed.
# SFML is not built from source: the prebuilt SFML 3.0 from the course template
# is used instead (it links against the X11/freetype/GL runtime libs already
# on the hub). Fall back to building SFML from source if none is found.
#
# Usage:  ./jupyterhub/run.sh
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT/app"

# Locate a prebuilt SFML 3 (e.g. the course template) and point CMake at it.
SFML_DIR="${SFML_DIR:-}"
if [ -z "$SFML_DIR" ]; then
    for c in \
        "$HOME"/cpp-sommersemester-*/Task_*_Vorlage/SFML \
        "$ROOT/third_party/SFML"; do
        for d in $c; do
            if compgen -G "$d/lib/libsfml-graphics*.so*" >/dev/null; then
                SFML_DIR="$d"
                break
            fi
        done
        [ -n "$SFML_DIR" ] && break
    done
fi

CMAKE_ARGS=(-DCMAKE_BUILD_TYPE=Release)
if [ -n "$SFML_DIR" ]; then
    echo "==> Using prebuilt SFML at $SFML_DIR"
    CMAKE_ARGS+=(-DCMAKE_PREFIX_PATH="$SFML_DIR")
    export LD_LIBRARY_PATH="$SFML_DIR/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
else
    echo "==> No prebuilt SFML found; building SFML from source"
fi

echo "==> Building"
cmake -S . -B build "${CMAKE_ARGS[@]}"
cmake --build build -j4

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