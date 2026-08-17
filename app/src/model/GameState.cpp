#include "GameState.hpp"
#include <fstream>

// Format a time in seconds as "MM:SS.hh" (minutes:seconds.hundredths).
// Negative values (no record yet) are shown as "NONE".
std::string GameState::fmt_time(float t) {
    if (t < 0) return "NONE";
    int mins = int(t) / 60;
    int secs = int(t) % 60;
    int hs = int((t - int(t)) * 100);
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%02d:%02d.%02d", mins, secs, hs);
    return buf;
}

// Read the persisted best times (9 stage times + overall time).
void GameState::load_records(const char* path) {
    std::ifstream f(path);
    if (!f.is_open()) return;
    for (int i = 0; i < 9; i++) f >> records.stage_times[i];
    f >> records.overall_time;
}

// Write the best times to disk so they survive between runs.
void GameState::save_records(const char* path) {
    std::ofstream f(path);
    if (!f.is_open()) return;
    for (int i = 0; i < 9; i++) f << records.stage_times[i] << "\n";
    f << records.overall_time;
}

// Difficulty-specific value lookup: each difficulty setting has its own value,
// and the custom difficulty returns the user-tuned slider value.
float GameState::diff_value(float easy, float normal, float hard, float custom) const {
    switch (difficulty) {
        case Difficulty::Easy:   return easy;
        case Difficulty::Normal: return normal;
        case Difficulty::Hard:   return hard;
        case Difficulty::Custom: return custom;
    }
    return normal;
}

// Gameplay tuning per difficulty. Higher difficulties mean faster and more
// frequent threats.
float GameState::get_barrel_speed()    const { return diff_value(120.f, 180.f, 260.f, custom_speed); }
float GameState::get_barrel_interval() const { return diff_value(3.0f, 2.0f, 1.2f, custom_interval); }
float GameState::get_pillar_speed()    const { return diff_value(60.f, 100.f, 150.f, custom_pillar_speed); }
float GameState::get_fire_interval()   const { return diff_value(3.5f, 2.5f, 1.3f, custom_fire_interval); }
float GameState::get_fireball_speed()  const { return diff_value(150.f, 250.f, 360.f, custom_fireball_speed); }