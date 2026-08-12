#ifndef DK_GAMESTATE_H
#define DK_GAMESTATE_H

#include <string>
#include <cstdio>

/// Holds per-stage and overall best times, persisted to disk.
struct TimeRecord {
    float stage_times[9] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
    float overall_time = -1;
};

/// Pure game state: phase, stage progress, timers, lives, records, difficulty.
class GameState {
public:
    enum class Phase { Title, Custom, Playing, GameOver, Won };
    enum class Difficulty { Easy, Normal, Hard, Custom };

    int stage = 1;
    int crowns = 0;
    int lives = 3;
    int last_pickup_stage = 0;
    bool paused = false;
    Phase phase = Phase::Title;
    float overall_timer = 0.f;
    float stage_timer = 0.f;
    float skip_cd = 0.f;
    bool muted = false;

    Difficulty difficulty = Difficulty::Normal;
    float custom_speed = 150.f;
    float custom_interval = 2.5f;
    float custom_pillar_speed = 100.f;
    float custom_fire_interval = 2.5f;
    float custom_fireball_speed = 250.f;

    TimeRecord records;

    float get_barrel_speed() const;
    float get_barrel_interval() const;
    float get_pillar_speed() const;
    float get_fire_interval() const;
    float get_fireball_speed() const;

    void save_records(const char* path = "assets/times.dat");
    void load_records(const char* path = "assets/times.dat");
    static std::string fmt_time(float t);
};

#endif
