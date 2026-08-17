cmake_minimum_required(VERSION 3.22)
project(LadderClimber LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Use a preinstalled SFML 3 if one is provided (e.g. the course template on the
# JupyterHub), otherwise download and build it from source.
include(FetchContent)
find_package(SFML 3 QUIET CONFIG COMPONENTS Graphics Audio)
if(NOT SFML_FOUND OR NOT SFML_Graphics_FOUND OR NOT SFML_Audio_FOUND)
    FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.0
        GIT_SHALLOW ON
        GIT_PROGRESS ON)
    FetchContent_MakeAvailable(SFML)
endif()

FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
    GIT_SHALLOW ON)
FetchContent_MakeAvailable(googletest)

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

target_include_directories(LadderClimber PRIVATE src)
target_compile_features(LadderClimber PRIVATE cxx_std_20)
target_link_libraries(LadderClimber PRIVATE SFML::Graphics SFML::Audio)

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
target_include_directories(LadderClimber_test PRIVATE src)
target_compile_features(LadderClimber_test PRIVATE cxx_std_20)
target_link_libraries(LadderClimber_test PRIVATE gtest_main SFML::Graphics SFML::Audio)

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
