cmake_minimum_required(VERSION 3.22)
project(LadderClimber LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Use a preinstalled SFML 3 if one is provided (e.g. the course template on the
# JupyterHub), otherwise download and build it from source.
include(FetchContent)
set(LC_PREBUILT_SFML OFF)
find_package(SFML 3 QUIET CONFIG COMPONENTS Graphics Audio)
if(NOT TARGET SFML::Graphics OR NOT TARGET SFML::Audio)
    FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.0
        GIT_SHALLOW ON
        GIT_PROGRESS ON)
    FetchContent_MakeAvailable(SFML)
else()
    set(LC_PREBUILT_SFML ON)
endif()

# The course template's prebuilt libsfml-audio.so carries unresolved vorbis/FLAC
# references (built without recording its own deps). When those runtime libs are
# available (LC_AUDIO_DEPS, filled in by jupyterhub/run.sh) they are linked to
# satisfy them; otherwise the game is built without music so it still links.
set(LC_AUDIO_DEPS "" CACHE STRING "Extra shared libs needed by prebuilt SFML audio")
set(LC_AUDIO_ENABLED ON)
if(LC_PREBUILT_SFML AND NOT LC_AUDIO_DEPS)
    set(LC_AUDIO_ENABLED OFF)
endif()

function(configure_ladder_target target)
    target_include_directories(${target} PRIVATE src)
    target_compile_features(${target} PRIVATE cxx_std_20)
    if(LC_AUDIO_ENABLED)
        target_link_libraries(${target} PRIVATE SFML::Graphics SFML::Audio ${LC_AUDIO_DEPS})
        target_compile_definitions(${target} PRIVATE LC_AUDIO=1)
    else()
        target_link_libraries(${target} PRIVATE SFML::Graphics)
        target_compile_definitions(${target} PRIVATE LC_AUDIO=0)
    endif()
endfunction()

add_executable(LadderClimber
    src/main.cpp
    src/Game.cpp
    src/model/GameState.cpp
    src/model/Player.cpp
    src/model/Platform.cpp
    src/model/Barrel.cpp
    src/model/Boss.cpp
    src/model/PowerUp.cpp
    src/model/PillarEnemy.cpp
    src/model/HeartPickup.cpp
    src/view/GameView.cpp
    src/control/GameController.cpp
)
configure_ladder_target(LadderClimber)

FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
    GIT_SHALLOW ON)
FetchContent_MakeAvailable(googletest)

# Tests
add_executable(LadderClimber_test
    src/model/GameState.cpp
    src/model/Player.cpp
    src/model/Barrel.cpp
    src/model/PowerUp.cpp
    src/model/Platform.cpp
    src/model/Boss.cpp
    src/model/PillarEnemy.cpp
    src/model/HeartPickup.cpp
    src/view/GameView.cpp
    src/control/GameController.cpp
    test/GameStateTest.cpp
    test/PlayerTest.cpp
    test/BarrelTest.cpp
    test/PowerUpTest.cpp
    test/PlatformTest.cpp
    test/BossTest.cpp
    test/GameControllerTest.cpp
    test/GameViewTest.cpp
)
configure_ladder_target(LadderClimber_test)
target_link_libraries(LadderClimber_test PRIVATE gtest_main)

# Copy assets into the build tree
set(FONT_CANDIDATES
    /usr/share/fonts/TTF/DejaVuSans.ttf
    /usr/share/fonts/truetype/dejavu/DejaVuSans.ttf
    /usr/share/fonts/Adwaita/AdwaitaSans-Regular.ttf)
foreach(F ${FONT_CANDIDATES})
    if(EXISTS "${F}")
        get_filename_component(FNAME "${F}" NAME)
        file(COPY "${F}" DESTINATION "${CMAKE_CURRENT_LIST_DIR}/assets/fonts/")
        file(RENAME "${CMAKE_CURRENT_LIST_DIR}/assets/fonts/${FNAME}" "${CMAKE_CURRENT_LIST_DIR}/assets/fonts/arial.ttf")
        break()
    endif()
endforeach()

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_LIST_DIR}/assets
        ${CMAKE_CURRENT_BINARY_DIR}/assets)
add_dependencies(LadderClimber copy_assets)
