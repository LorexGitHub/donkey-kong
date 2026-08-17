#include "Game.hpp"
#include <cmath>
#include <fstream>
#include <cstdlib>
#include <ctime>

Game::Game()
    : controller(view, state, player, ladders, music,
                 [this]() { start_game(); },
                 [this]() { play_random_music(); },
                 [this]() { go_to_title(); }) {
    std::srand((unsigned)std::time(nullptr));
    state.load_records();
    go_to_title();
}

void Game::go_to_title() {
    state.phase = GameState::Phase::Title;
    state.paused = false;
    state.overall_timer = 0;
    state.lives = 3;
    state.crowns = 0;  // Reset progress so a new run starts from stage 1
    state.last_pickup_stage = 0;
    barrels.clear();
    powerup.reset();
    heart_pickup.reset();
    player.set_position(100.f, 710.f);
    player.set_dead(false);
    player.set_climbing(false);
    play_random_music();
}

void Game::play_random_music() {
    music.stop();
    static const char* tracks[] = {
        "assets/music/track1.mp3",  "assets/music/track2.mp3",
        "assets/music/track3.mp3",  "assets/music/track4.mp3",
        "assets/music/track5.mp3",  "assets/music/track6.mp3",
        "assets/music/track7.mp3",  "assets/music/track8.mp3",
        "assets/music/track9.mp3",  "assets/music/track10.mp3",
        "assets/music/track11.mp3", "assets/music/track12.mp3",
    };
    std::vector<std::string> songs;
    for (const char* p : tracks) {
        std::ifstream f(p);
        if (f.is_open()) songs.push_back(p);
    }
    if (!songs.empty()) {
        int idx = std::rand() % songs.size();
        if (music.openFromFile(songs[idx])) {
            music.setLooping(true);
            music.setVolume(state.muted ? 0.f : 15.f);
            music.play();
        }
    }
}

void Game::start_game() {
    if (state.crowns >= 9)
        state.crowns = 0;
    setup_stage();
    if (state.stage != state.last_pickup_stage) {
        spawn_pickups();
        state.last_pickup_stage = state.stage;
    }
    state.phase = GameState::Phase::Playing;
    state.paused = false;
    barrels.clear();
    barrel_timer = 0;
    player.set_position(100.f, 710.f);
    player.set_dead(false);
    player.set_invincible(0.f);  // Reset invincibility
    dk = Boss(60.f, 140.f, state.stage);
    player.set_climbing(false);

    float speed = state.get_barrel_speed();
    for (int lvl = 0; lvl < (int)platforms.size() - 1; lvl++) {
        float y = platforms[lvl].get_bounds().position.y;
        if (lvl % 2 == 0)
            barrels.push_back(std::make_unique<Barrel>(60.f, y, speed, lvl));
        else
            barrels.push_back(std::make_unique<Barrel>(728.f, y, -speed, lvl));
    }
    state.stage_timer = 0;
}

void Game::setup_stage() {
    platforms.clear();
    ladders.clear();

    state.stage = state.crowns + 1;

    std::vector<float> ys;
    if (state.stage == 1)
        ys = {140.f, 710.f};
    else if (state.stage == 2)
        ys = {140.f, 330.f, 520.f, 710.f};
    else
        ys = {140.f, 254.f, 368.f, 482.f, 596.f, 710.f};

    for (float y : ys)
        platforms.emplace_back(50.f, y, 700.f);

    int n = ys.size();
    for (int i = 0; i < n - 1; i++) {
        float x = ((n - 2 - i) % 2 == 0) ? 680.f : 130.f;
        ladders.emplace_back(x, ys[i], ys[i + 1]);
    }

    // Create pillar enemies
    float top_y = ys.front();
    float bot_y = ys.back();
    left_pillar = std::make_unique<PillarEnemy>(35.f, top_y, bot_y, 1);
    right_pillar = std::make_unique<PillarEnemy>(765.f, top_y, bot_y, -1);

    float pspeed = state.get_pillar_speed();
    float pfire  = state.get_fire_interval();
    float pfball = state.get_fireball_speed();
    left_pillar->set_move_speed(pspeed);
    left_pillar->set_fire_interval(pfire);
    left_pillar->set_fireball_speed(pfball);
    right_pillar->set_move_speed(pspeed);
    right_pillar->set_fire_interval(pfire);
    right_pillar->set_fireball_speed(pfball);

    if (state.stage >= 4) {
        int num_with_holes = state.stage - 3;
        if (num_with_holes > (int)platforms.size())
            num_with_holes = (int)platforms.size();
        for (int h = 0; h < num_with_holes; h++) {
            int pi = (state.crowns * 7 + h * 13) % (int)platforms.size();
            int seg = ((pi * 100 + state.crowns * 50 + h * 37) % 8 + 8) % 8 + 1;
            if (!platforms[pi].is_solid(seg)) {
                pi = (pi + 1) % (int)platforms.size();
                seg = ((pi * 100 + state.crowns * 50) % 8 + 8) % 8 + 1;
            }
            platforms[pi].set_solid(seg, false);
        }
    }

}

void Game::run() {
    sf::Clock clock;
    while (view.window.isOpen()) {
        sf::Time dt = clock.restart();
        controller.handle_input(dt.asSeconds());
        if (state.phase == GameState::Phase::Playing && !state.paused)
            update(dt.asSeconds());
        view.draw(state, player, platforms, ladders, barrels, dk, lava_anim, powerup.get(), heart_pickup.get(), left_pillar.get(), right_pillar.get());
    }
}

void Game::update(float dt) {
    lava_anim += dt * 30.f;
    if (lava_anim > 800) lava_anim = 0;
    state.overall_timer += dt;
    state.stage_timer += dt;

    player.update(dt);

    // Keep the player inside the side bars (where flame shooters run)
    float half = player.get_bounds().size.x / 2.f;
    float bar_left = (left_pillar ? left_pillar->get_pos().x : 35.f) + half;
    float bar_right = (right_pillar ? right_pillar->get_pos().x : 765.f) - half;
    player.clamp_x(bar_left, bar_right);

    // Ladder top/bottom clamping
    if (player.is_climbing()) {
        for (auto& l : ladders) {
            if (player.get_bounds().findIntersection(l.get_bounds()).has_value()) {
                float ltop = l.get_bounds().position.y;
                float lbot = ltop + l.get_bounds().size.y;
                float py = player.get_pos().y;
                if (py < ltop) {
                    player.set_position(player.get_pos().x, ltop);
                    player.set_climbing(false);
                }
                if (py > lbot) {
                    player.set_position(player.get_pos().x, lbot);
                    player.set_climbing(false);
                }
                break;
            }
        }
    }

    // Player-platform collision using segments
    if (!player.is_climbing()) {
        bool on_any = false;
        for (int pi = 0; pi < (int)platforms.size(); pi++) {
            auto overlap = player.get_bounds().findIntersection(platforms[pi].get_bounds());
            if (!overlap.has_value()) continue;

            auto ppos = platforms[pi].get_bounds();
            float player_bottom = player.get_pos().y + 16;
            float platform_top = ppos.position.y;

            if (player_bottom >= platform_top && player_bottom < platform_top + 24 && player.get_vel().y >= 0) {
                if (platforms[pi].solid_segment_hit(player.get_bounds())) {
                    float p_left  = ppos.position.x + 12;
                    float p_right = ppos.position.x + ppos.size.x - 12;
                    float px = player.get_pos().x;
                    if (px < p_left)  px = p_left;
                    if (px > p_right) px = p_right;
                    player.set_position(px, platform_top);
                    on_any = true;
                    break;
                }
            }
        }
        if (!on_any) {
            player.set_on_ground(false);
            for (int pi = 0; pi < (int)platforms.size(); pi++) {
                auto overlap = player.get_bounds().findIntersection(platforms[pi].get_bounds());
                if (!overlap.has_value()) continue;

                auto ppos = platforms[pi].get_bounds();
                float player_head = player.get_pos().y - 32;
                float platform_bot = ppos.position.y + 14;

                if (player.get_vel().y < 0 && player.get_pos().y > ppos.position.y &&
                    player_head <= platform_bot && player_head > platform_bot - 16) {
                    if (platforms[pi].solid_segment_hit(player.get_bounds())) {
                        player.bump_head(platform_bot + 32);
                        break;
                    }
                }
            }
        }
    }

    // Barrels
    barrel_timer += dt;
    if (barrel_timer >= state.get_barrel_interval()) {
        spawn_barrel();
        barrel_timer = 0;
    }

    for (auto it = barrels.begin(); it != barrels.end();) {
        (*it)->update(dt);

        for (int pi = 0; pi < (int)platforms.size(); pi++) {
            auto ov = (*it)->get_bounds().findIntersection(platforms[pi].get_bounds());
            if (ov.has_value()) {
                float ptop = platforms[pi].get_bounds().position.y;
                float bbot = (*it)->get_pos().y + 12;
                if (bbot >= ptop - 4 && bbot < ptop + 20) {
                    if (platforms[pi].solid_segment_hit((*it)->get_bounds())) {
                        (*it)->land_on_surface(ptop - 12);
                        (*it)->set_level(pi);
                        float bs = state.get_barrel_speed();
                        (*it)->set_velocity((pi % 2 == 0) ? bs : -bs, 0);
                        goto barrel_landed;
                    }
                }
            }
        }
        barrel_landed:;

        int lvl = (*it)->get_level();
        if (lvl >= 0 && lvl < (int)platforms.size()) {
            float bx = (*it)->get_pos().x;
            bool at_edge = (lvl % 2 == 0) ? (bx > 740) : (bx < 60);
            if (at_edge && lvl < (int)platforms.size() - 1) {
                int next = lvl + 1;
                float next_y = platforms[next].get_bounds().position.y;
                float new_x = (next % 2 == 0) ? 60.f : 728.f;
                float bs = state.get_barrel_speed();
                float new_vx = (next % 2 == 0) ? bs : -bs;
                (*it)->set_position(new_x, next_y);
                (*it)->set_velocity(new_vx, 0);
                (*it)->set_level(next);
            }
        }

        if (!(*it)->is_alive())
            it = barrels.erase(it);
        else
            ++it;
    }

    // Pillar enemies
    if (left_pillar) left_pillar->update(dt);
    if (right_pillar) right_pillar->update(dt);

    // HeartPickup
    if (heart_pickup) {
        heart_pickup->update(dt);
        if (heart_pickup->is_active() && player.get_bounds().findIntersection(heart_pickup->get_bounds()).has_value()) {
            heart_pickup->collect();
            if (state.lives < 3) state.lives++;  // Max 3 hearts
        }
    }

    // PowerUp
    if (powerup) {
        powerup->update(dt);
        if (powerup->is_active() && player.get_bounds().findIntersection(powerup->get_bounds()).has_value()) {
            powerup->collect();
            player.set_invincible(5.f);
        }
    }

    check_collisions();

    // Lava kills
    if (player.get_pos().y > 710 && state.phase == GameState::Phase::Playing) {
        lose_life();
    }

    // Win: player reached top platform near princess
    if (!platforms.empty()) {
        float top_y = platforms[0].get_bounds().position.y;
        if (player.get_pos().y < top_y + 30 && player.get_pos().x < 100 && state.phase == GameState::Phase::Playing) {
            state.crowns++;
            int si = state.stage - 1;
            if (state.stage_timer < state.records.stage_times[si] || state.records.stage_times[si] < 0)
                state.records.stage_times[si] = state.stage_timer;
            if (state.crowns >= 9) {
                if (state.overall_timer < state.records.overall_time || state.records.overall_time < 0)
                    state.records.overall_time = state.overall_timer;
            }
            state.save_records();
            state.phase = GameState::Phase::Won;
        }
    }
}

void Game::check_collisions() {
    if (player.is_invincible()) return;

    // Fireballs from pillar enemies
    for (auto& e : {left_pillar.get(), right_pillar.get()}) {
        if (!e) continue;
        for (auto& fb : e->get_fireballs()) {
            if (!fb.alive) continue;
            sf::FloatRect fb_bounds{{fb.pos.x - 5.f, fb.pos.y - 5.f}, {10.f, 10.f}};
            if (player.get_bounds().findIntersection(fb_bounds).has_value()) {
                fb.alive = false;
                lose_life();
                return;
            }
        }
    }

    for (auto& b : barrels) {
        if (player.get_bounds().findIntersection(b->get_bounds()).has_value()) {
            lose_life();
            return;
        }
    }
}

// Lose one life; on the last one the run ends, otherwise the stage restarts
// with a mercy power-up placed near the player.
void Game::lose_life() {
    state.lives--;
    if (state.lives <= 0) {
        state.phase = GameState::Phase::GameOver;
        player.set_dead(true);
        state.crowns = 0;
        play_random_music();
    } else {
        auto ppos = player.get_pos();
        place_god_powerup(ppos.x);
        start_game();
    }
}

void Game::spawn_barrel() {
    if (platforms.empty()) return;
    float y = platforms[0].get_bounds().position.y;
    barrels.push_back(std::make_unique<Barrel>(60.f, y, state.get_barrel_speed(), 0));
}

// Place the god-mode pickup just above whichever platform spans this x-position.
// Falls back to the bottom (spawn) platform so it is always reachable.
void Game::place_god_powerup(float px) {
    float top_y = platforms.empty() ? 710.f : platforms.back().get_bounds().position.y;
    for (auto& p : platforms) {
        auto pb = p.get_bounds();
        if (px >= pb.position.x && px <= pb.position.x + pb.size.x) {
            top_y = pb.position.y;
            break;
        }
    }
    powerup = std::make_unique<PowerUp>(px - 12.f, top_y - 14.f);
}

void Game::spawn_pickups() {
    powerup.reset();
    heart_pickup.reset();

    // Invincible pickup (god mode) only on stage 1, in the middle of the spawn platform.
    if (state.stage == 1) {
        auto spawn_pb = platforms.back().get_bounds();
        float center_x = spawn_pb.position.x + spawn_pb.size.x / 2.f;
        place_god_powerup(center_x);
    }

    // Heart pickup - one per stage, on a different platform
    int ppi = (int)platforms.size() - 1;
    int hpi = std::rand() % (int)(platforms.size() - 1) + 1;
    if (hpi == ppi && platforms.size() > 2) hpi = (hpi + 1) % (int)platforms.size();
    auto hpb = platforms[hpi].get_bounds();
    float hx = hpb.position.x + 60.f + (std::rand() % 580);
    heart_pickup = std::make_unique<HeartPickup>(hx, hpb.position.y - 10.f);
}
