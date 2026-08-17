#include "GameView.hpp"
#include "../model/GameState.hpp"
#include "../model/Player.hpp"
#include "../model/Platform.hpp"
#include "../model/Barrel.hpp"
#include "../model/Boss.hpp"
#include "../model/Pickup.hpp"
#include "../model/PillarEnemy.hpp"
#include <cmath>

// ── Layout constants for title screen ──
static sf::FloatRect r_easy()   { return {{160, 260}, {130, 50}}; }
static sf::FloatRect r_normal() { return {{335, 260}, {130, 50}}; }
static sf::FloatRect r_hard()   { return {{510, 260}, {130, 50}}; }
static sf::FloatRect r_custom() { return {{320, 340}, {160, 50}}; }
static sf::FloatRect r_play_t() { return {{320, 430}, {160, 50}}; }

// ── Layout constants for custom screen (grouped by enemy) ──
static sf::FloatRect r_speed()        { return {{250, 205}, {300, 20}}; }
static sf::FloatRect r_interval()     { return {{250, 275}, {300, 20}}; }
static sf::FloatRect r_pillar_speed() { return {{250, 420}, {300, 20}}; }
static sf::FloatRect r_fire_rate()    { return {{250, 490}, {300, 20}}; }
static sf::FloatRect r_fire_speed()   { return {{250, 560}, {300, 20}}; }
static sf::FloatRect r_cplay()        { return {{320, 650}, {160, 50}}; }

sf::FloatRect GameView::title_btn_easy()   { return r_easy(); }
sf::FloatRect GameView::title_btn_normal() { return r_normal(); }
sf::FloatRect GameView::title_btn_hard()   { return r_hard(); }
sf::FloatRect GameView::title_btn_custom() { return r_custom(); }
sf::FloatRect GameView::title_btn_play()   { return r_play_t(); }
sf::FloatRect GameView::custom_speed_track()       { return r_speed(); }
sf::FloatRect GameView::custom_interval_track()    { return r_interval(); }
sf::FloatRect GameView::custom_pillar_speed_track(){ return r_pillar_speed(); }
sf::FloatRect GameView::custom_fire_rate_track()   { return r_fire_rate(); }
sf::FloatRect GameView::custom_fire_speed_track()  { return r_fire_speed(); }
sf::FloatRect GameView::custom_btn_play()          { return r_cplay(); }

GameView::GameView() {
    window.create(sf::VideoMode({800u, 750u}), "Ladder Climber", sf::Style::Resize | sf::Style::Close);
    window.setMinimumSize(sf::Vector2u{800u, 750u});
    window.setMaximumSize(sf::Vector2u{1600u, 1500u});
    auto dsize = sf::VideoMode::getDesktopMode().size;
    window.setPosition({
        (int)((dsize.x - 800u) / 2u),
        (int)((dsize.y - 750u) / 2u)
    });

    window.setFramerateLimit(60);
    load_font();
    update_view();

    title_text.setString("LADDER CLIMBER");
    title_text.setCharacterSize(48);
    title_text.setFillColor(sf::Color::Yellow);
    auto tb = title_text.getLocalBounds();
    title_text.setOrigin({tb.position.x + tb.size.x / 2, tb.position.y + tb.size.y / 2});
    title_text.setPosition({400, 100});

    status_text.setCharacterSize(40);
    status_text.setFillColor(sf::Color::Yellow);

    menu_btn.setSize({200, 60});
    menu_btn.setFillColor(sf::Color{50, 150, 50});
    menu_btn.setOrigin({100, 30});
    menu_btn.setPosition({400, 340});
    menu_btn_text.setString("PLAY");
    menu_btn_text.setCharacterSize(30);
    menu_btn_text.setFillColor(sf::Color::White);
    auto mt = menu_btn_text.getLocalBounds();
    menu_btn_text.setOrigin({mt.position.x + mt.size.x / 2, mt.position.y + mt.size.y / 2});
    menu_btn_text.setPosition({400, 340});

    auto setup_btn = [](sf::RectangleShape& s, sf::Text& t, float x, float y, const std::string& label) {
        s.setSize({160, 50});
        s.setFillColor(sf::Color{60, 60, 180});
        s.setOrigin({80, 25});
        s.setPosition({x, y});
        t.setString(label);
        t.setCharacterSize(24);
        t.setFillColor(sf::Color::White);
        auto lb = t.getLocalBounds();
        t.setOrigin({lb.position.x + lb.size.x / 2, lb.position.y + lb.size.y / 2});
        t.setPosition({x, y});
    };
    setup_btn(pause_resume_btn, pause_resume_text, 400, 260, "RESUME");
    setup_btn(pause_reset_btn, pause_reset_text, 400, 340, "MENU");

    princess.setSize({20, 36});
    princess.setFillColor(sf::Color::Magenta);
    princess.setPosition({70, 104});
    [[maybe_unused]] bool pt_loaded = princess_tex.loadFromFile("assets/sprites/princess.png");

    crowns_text.setCharacterSize(24);
    crowns_text.setFillColor(sf::Color::Yellow);

    mute_btn.setSize({22, 22});
    mute_btn.setFillColor(sf::Color{80, 80, 80, 180});
    mute_btn.setPosition({8, 10});
    mute_text.setString("M");
    mute_text.setCharacterSize(14);
    mute_text.setFillColor(sf::Color::White);
    auto mb = mute_text.getLocalBounds();
    mute_text.setOrigin({mb.position.x + mb.size.x / 2, mb.position.y + mb.size.y / 2});
    mute_text.setPosition({19, 21});

    time_text.setCharacterSize(14);
    time_text.setFillColor(sf::Color::White);
    record_text.setCharacterSize(14);
    record_text.setFillColor(sf::Color::White);
    overall_rec_text.setCharacterSize(14);
    overall_rec_text.setFillColor(sf::Color::Yellow);
    stage_rec_text.setCharacterSize(14);
    stage_rec_text.setFillColor(sf::Color::Yellow);

    [[maybe_unused]] bool bg_ok = bg_tex.loadFromFile("assets/sprites/background.png");
    [[maybe_unused]] bool p_ok = plat_tex.loadFromFile("assets/sprites/platform.png");
    [[maybe_unused]] bool lt_ok = ladder_tex.loadFromFile("assets/sprites/ladder.png");
    [[maybe_unused]] bool lv_ok = lava_tex.loadFromFile("assets/sprites/lava.png");
    [[maybe_unused]] bool boss_ok = boss_tex.loadFromFile("assets/sprites/boss.png");
    bg_shape.setFillColor(sf::Color{20, 20, 40});
}

void GameView::update_view() {
    sf::Vector2u ws = window.getSize();
    float w = (float)ws.x, h = (float)ws.y;
    if (w < 1 || h < 1) return;
    // Letterbox: use the smallest uniform scale so the whole game is always
    // visible, centered. When the window aspect ratio differs from 4:3 the
    // leftover space shows as black bars on the sides (or top/bottom).
    float scale = std::min(w / 800.f, h / 750.f);
    sf::FloatRect vp{
        {w - 800.f * scale, h - 750.f * scale},
        {w, h}};
    vp.position.x /= 2.f * w;
    vp.position.y /= 2.f * h;
    vp.size.x = 800.f * scale / w;
    vp.size.y = 750.f * scale / h;
    game_view = sf::View(sf::FloatRect{{0.f, 0.f}, {800.f, 750.f}});
    game_view.setViewport(vp);
    window.setView(game_view);
}

bool GameView::load_font() {
    if (font.openFromFile("assets/fonts/arial.ttf")) return true;
    if (font.openFromFile("assets/fonts/DejaVuSans.ttf")) return true;
    if (font.openFromFile("/usr/share/fonts/TTF/DejaVuSans.ttf")) return true;
    if (font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) return true;
    if (font.openFromFile("/usr/share/fonts/Adwaita/AdwaitaSans-Regular.ttf")) return true;
    return false;
}

sf::FloatRect GameView::get_menu_btn_bounds() const {
    return menu_btn.getGlobalBounds();
}

sf::FloatRect GameView::get_pause_resume_btn_bounds() const {
    return pause_resume_btn.getGlobalBounds();
}

sf::FloatRect GameView::get_pause_reset_btn_bounds() const {
    return pause_reset_btn.getGlobalBounds();
}

sf::FloatRect GameView::get_mute_btn_bounds() const {
    return mute_btn.getGlobalBounds();
}

sf::FloatRect GameView::draw_btn(sf::FloatRect rect, const std::string& label, const sf::Color& color) {
    sf::RectangleShape btn({rect.size.x, rect.size.y});
    btn.setPosition({rect.position.x, rect.position.y});
    btn.setFillColor(color);
    window.draw(btn);
    sf::Text txt(font, label, 24);
    txt.setFillColor(sf::Color::White);
    auto tb = txt.getLocalBounds();
    txt.setOrigin({tb.position.x + tb.size.x / 2, tb.position.y + tb.size.y / 2});
    txt.setPosition({rect.position.x + rect.size.x / 2, rect.position.y + rect.size.y / 2});
    window.draw(txt);
    return rect;
}

float GameView::draw_slider(float cx, float cy, float min_val, float max_val, float val) {
    float track_w = 300.f;
    float norm = (val - min_val) / (max_val - min_val);
    float hx = cx - track_w / 2.f + norm * track_w;

    sf::RectangleShape track({track_w, 6.f});
    track.setOrigin({track_w / 2.f, 3.f});
    track.setPosition({cx, cy});
    track.setFillColor(sf::Color{100, 100, 120});
    window.draw(track);

    sf::CircleShape handle(8.f);
    handle.setOrigin({8.f, 8.f});
    handle.setPosition({hx, cy});
    handle.setFillColor(sf::Color{200, 200, 100});
    handle.setOutlineColor(sf::Color::White);
    handle.setOutlineThickness(2.f);
    window.draw(handle);

    return hx;
}

void GameView::draw(const GameState& state, const Player& player,
                    const std::vector<Platform>& platforms,
                    const std::vector<Ladder>& ladders,
                    const std::vector<std::unique_ptr<Barrel>>& barrels,
                    const Boss& dk, float lava_anim,
                    const Pickup* powerup,
                    const Pickup* heart_pickup,
                    const PillarEnemy* left_pillar,
                    const PillarEnemy* right_pillar) {

    window.clear(sf::Color{20, 20, 40});
    window.setView(game_view);

    // ── TITLE SCREEN ──
    if (state.phase == GameState::Phase::Title) {
        // Background
        if (bg_tex.getSize().x > 0) {
            sf::Sprite bg_spr(bg_tex);
            window.draw(bg_spr);
        } else {
            window.draw(bg_shape);
        }

        // Boss + princess icons near the top of the level
        if (boss_tex.getSize().x > 0) {
            sf::Sprite boss_spr(boss_tex);
            auto bs = boss_tex.getSize();
            float bscale = 48.f / (float)(bs.x > 0 ? bs.x : 48);
            boss_spr.setScale({bscale, bscale});
            boss_spr.setOrigin({(float)bs.x / 2.f, (float)bs.y / 2.f});
            boss_spr.setPosition({90, 140});
            window.draw(boss_spr);
        }
        if (princess_tex.getSize().x > 0) {
            sf::Sprite pr_spr(princess_tex);
            auto ps = princess_tex.getSize();
            float pscale = 30.f / (float)(ps.x > 0 ? ps.x : 30);
            pr_spr.setScale({pscale, pscale});
            pr_spr.setOrigin({(float)ps.x / 2.f, (float)ps.y / 2.f});
            pr_spr.setPosition({120, 156});
            window.draw(pr_spr);
        }

        sf::RectangleShape overlay({800, 750});
        overlay.setFillColor(sf::Color{0, 0, 0, 150});
        window.draw(overlay);
        window.draw(title_text);

        // Difficulty buttons
        auto is_sel = [&](GameState::Difficulty d) { return state.difficulty == d; };
        auto diff_color = [&](GameState::Difficulty d) {
            return is_sel(d) ? sf::Color{80, 180, 80} : sf::Color{60, 60, 140};
        };
        draw_btn(r_easy(),   "EASY",   diff_color(GameState::Difficulty::Easy));
        draw_btn(r_normal(), "NORMAL", diff_color(GameState::Difficulty::Normal));
        draw_btn(r_hard(),   "HARD",   diff_color(GameState::Difficulty::Hard));
        draw_btn(r_custom(), "CUSTOM", diff_color(GameState::Difficulty::Custom));
        draw_btn(r_play_t(), "PLAY",   sf::Color{50, 150, 50});

        window.display();
        return;
    }

    // ── CUSTOM DIFFICULTY SCREEN ──
    if (state.phase == GameState::Phase::Custom) {
        sf::RectangleShape overlay({800, 750});
        overlay.setFillColor(sf::Color{0, 0, 0, 200});
        window.draw(overlay);

        sf::Text heading(font, "CUSTOM DIFFICULTY", 32);
        heading.setFillColor(sf::Color::Yellow);
        auto hb = heading.getLocalBounds();
        heading.setOrigin({hb.position.x + hb.size.x / 2, hb.position.y + hb.size.y / 2});
        heading.setPosition({400, 55});
        window.draw(heading);

        // Helper to draw a section header with a sprite on the left
        auto draw_enemy_header = [&](float y, const std::string& label, const sf::Texture& tex) {
            sf::Text sec(font, label, 26);
            sec.setFillColor(sf::Color{255, 200, 100});
            auto sb = sec.getLocalBounds();
            sec.setOrigin({sb.position.x + sb.size.x / 2, sb.position.y + sb.size.y / 2});
            float sec_w = sb.size.x + 34.f;
            sec.setPosition({400 + 17.f, y});
            window.draw(sec);
            sf::Sprite spr(tex);
            auto ts = tex.getSize();
            float scale = 30.f / (float)(ts.x > 0 ? ts.x : 30);
            spr.setScale({scale, scale});
            spr.setOrigin({(float)ts.x / 2.f, (float)ts.y / 2.f});
            spr.setPosition({400 - sec_w / 2.f + 10.f, y});
            window.draw(spr);
        };

        // ── BOSS section (barrel conditions) ──
        {
            draw_enemy_header(100, "BOSS", boss_tex);

            sf::Text lbl1(font, "Barrel Speed:", 18);
            lbl1.setFillColor(sf::Color::White);
            auto lb1 = lbl1.getLocalBounds();
            lbl1.setOrigin({lb1.position.x + lb1.size.x / 2, 0});
            lbl1.setPosition({400, 130});
            window.draw(lbl1);
            draw_slider(400, 215, 60.f, 400.f, state.custom_speed);
            sf::Text val1(font, std::to_string(int(state.custom_speed)), 16);
            val1.setFillColor(sf::Color{200, 200, 100});
            auto vb1 = val1.getLocalBounds();
            val1.setOrigin({vb1.position.x + vb1.size.x / 2, 0});
            val1.setPosition({400, 235});
            window.draw(val1);

            sf::Text lbl2(font, "Barrel Rate (seconds):", 18);
            lbl2.setFillColor(sf::Color::White);
            auto lb2 = lbl2.getLocalBounds();
            lbl2.setOrigin({lb2.position.x + lb2.size.x / 2, 0});
            lbl2.setPosition({400, 265});
            window.draw(lbl2);
            draw_slider(400, 285, 0.5f, 5.f, state.custom_interval);
            char ibuf[16];
            std::snprintf(ibuf, sizeof(ibuf), "%.1f s", (double)state.custom_interval);
            sf::Text val2(font, ibuf, 16);
            val2.setFillColor(sf::Color{200, 200, 100});
            auto vb2 = val2.getLocalBounds();
            val2.setOrigin({vb2.position.x + vb2.size.x / 2, 0});
            val2.setPosition({400, 305});
            window.draw(val2);
        }

        // ── BAR ENEMY section (fire shooter conditions) ──
        {
            sf::Text sec2(font, "BAR ENEMY", 26);
            sec2.setFillColor(sf::Color{255, 200, 100});
            auto s2b = sec2.getLocalBounds();
            sec2.setOrigin({s2b.position.x + s2b.size.x / 2, s2b.position.y + s2b.size.y / 2});
            float sec2_w = s2b.size.x + 34.f;
            sec2.setPosition({400 + 17.f, 340});
            window.draw(sec2);
            sf::CircleShape bar_icon(13.f);
            bar_icon.setOrigin({13.f, 13.f});
            bar_icon.setPosition({400 - sec2_w / 2.f + 10.f, 340});
            bar_icon.setFillColor(sf::Color{220, 100, 20});
            bar_icon.setOutlineColor(sf::Color::Yellow);
            bar_icon.setOutlineThickness(2.f);
            window.draw(bar_icon);

            sf::Text lbl3(font, "Move Speed (up/down):", 18);
            lbl3.setFillColor(sf::Color::White);
            auto lb3 = lbl3.getLocalBounds();
            lbl3.setOrigin({lb3.position.x + lb3.size.x / 2, 0});
            lbl3.setPosition({400, 370});
            window.draw(lbl3);
            draw_slider(400, 430, 50.f, 250.f, state.custom_pillar_speed);
            sf::Text val3(font, std::to_string(int(state.custom_pillar_speed)), 16);
            val3.setFillColor(sf::Color{200, 200, 100});
            auto vb3 = val3.getLocalBounds();
            val3.setOrigin({vb3.position.x + vb3.size.x / 2, 0});
            val3.setPosition({400, 450});
            window.draw(val3);

            sf::Text lbl4(font, "Shoot Rate (seconds):", 18);
            lbl4.setFillColor(sf::Color::White);
            auto lb4 = lbl4.getLocalBounds();
            lbl4.setOrigin({lb4.position.x + lb4.size.x / 2, 0});
            lbl4.setPosition({400, 480});
            window.draw(lbl4);
            draw_slider(400, 500, 0.5f, 5.f, state.custom_fire_interval);
            char fbuf[16];
            std::snprintf(fbuf, sizeof(fbuf), "%.1f s", (double)state.custom_fire_interval);
            sf::Text val4(font, fbuf, 16);
            val4.setFillColor(sf::Color{200, 200, 100});
            auto vb4 = val4.getLocalBounds();
            val4.setOrigin({vb4.position.x + vb4.size.x / 2, 0});
            val4.setPosition({400, 520});
            window.draw(val4);

            sf::Text lbl5(font, "Fireball Speed:", 18);
            lbl5.setFillColor(sf::Color::White);
            auto lb5 = lbl5.getLocalBounds();
            lbl5.setOrigin({lb5.position.x + lb5.size.x / 2, 0});
            lbl5.setPosition({400, 550});
            window.draw(lbl5);
            draw_slider(400, 570, 120.f, 450.f, state.custom_fireball_speed);
            sf::Text val5(font, std::to_string(int(state.custom_fireball_speed)), 16);
            val5.setFillColor(sf::Color{200, 200, 100});
            auto vb5 = val5.getLocalBounds();
            val5.setOrigin({vb5.position.x + vb5.size.x / 2, 0});
            val5.setPosition({400, 590});
            window.draw(val5);
        }

        draw_btn(r_cplay(), "PLAY", sf::Color{50, 150, 50});

        window.display();
        return;
    }

    // ── GAME BACKGROUND ──
    if (bg_tex.getSize().x > 0) {
        sf::Sprite bg_spr(bg_tex);
        window.draw(bg_spr);
    } else {
        window.draw(bg_shape);
    }

    // Stage color tint overlay
    {
        sf::Color stage_tints[] = {
            {60, 20, 30, 35},   {20, 50, 40, 35},   {30, 20, 60, 35},
            {60, 50, 10, 35},   {50, 20, 60, 35},   {10, 50, 60, 35},
            {60, 40, 10, 35},   {40, 20, 55, 35}
        };
        sf::RectangleShape tint_overlay({800, 750});
        tint_overlay.setFillColor(stage_tints[(state.stage - 1) % 8]);
        window.draw(tint_overlay);
    }

    // Animated torches
    {
        int flicker = int(lava_anim) % 6;
        float fh = 10.f + flicker * 1.5f;
        auto draw_torch = [&](float tx, float ty) {
            sf::RectangleShape post({3, 14});
            post.setFillColor(sf::Color{80, 60, 30});
            post.setPosition({tx - 1, ty - 14});
            window.draw(post);
            sf::RectangleShape flame({8, fh});
            flame.setPosition({tx - 4, ty - 14 - fh});
            sf::Color fc(255, 120 + flicker * 15, 30);
            if (flicker < 2 || flicker > 4) fc = sf::Color(255, 80, 20);
            flame.setFillColor(fc);
            window.draw(flame);
        };
        draw_torch(77, 140);
        draw_torch(722, 140);
        for (size_t pi = 0; pi < platforms.size(); pi++) {
            float py = platforms[pi].get_bounds().position.y;
            draw_torch(100, py);
            draw_torch(700, py);
        }
    }

    // Lava
    if (lava_tex.getSize().x > 0) {
        sf::Sprite lava_spr(lava_tex);
        lava_spr.setPosition({0.f, 710.f});
        window.draw(lava_spr);
        sf::RectangleShape glow({800, 40});
        glow.setPosition({0, 710});
        int a = 20 + int(15 * std::sin(lava_anim * 0.1));
        glow.setFillColor(sf::Color(std::uint8_t(255), std::uint8_t(200), std::uint8_t(50), std::uint8_t(a)));
        window.draw(glow);
    } else {
        sf::RectangleShape lava_fill({800, 40});
        lava_fill.setPosition({0, 710});
        lava_fill.setFillColor(sf::Color{200, 50, 10});
        window.draw(lava_fill);
    }

    // Platforms with holes
    auto is_solid = [&](int pi, int s) {
        return pi < (int)platforms.size() && s < 10 && platforms[pi].is_solid(s);
    };

    if (plat_tex.getSize().x > 0) {
        for (int pi = 0; pi < (int)platforms.size(); pi++) {
            auto& p = platforms[pi];
            auto ppos = p.get_bounds().position;
            sf::Sprite p_spr(plat_tex);
            p_spr.setPosition(ppos);
            window.draw(p_spr);
            for (int s = 0; s < 10; s++) {
                if (is_solid(pi, s)) continue;
                sf::RectangleShape hole({70, 14});
                hole.setPosition({ppos.x + s * 70.f, ppos.y});
                hole.setFillColor(sf::Color{5, 5, 15});
                window.draw(hole);
            }
        }
    } else {
        for (int pi = 0; pi < (int)platforms.size(); pi++) {
            auto& p = platforms[pi];
            auto ppos = p.get_bounds().position;
            sf::RectangleShape seg_shape({70, 14});
            seg_shape.setFillColor(sf::Color{139, 69, 19});
            for (int s = 0; s < 10; s++) {
                if (!is_solid(pi, s)) continue;
                seg_shape.setPosition({ppos.x + s * 70.f, ppos.y});
                window.draw(seg_shape);
            }
        }
    }

    // Ladders
    if (ladder_tex.getSize().x > 0) {
        for (auto& l : ladders) {
            sf::Sprite l_spr(ladder_tex);
            l_spr.setPosition({l.get_bounds().position.x, l.get_bounds().position.y});
            l_spr.setScale({1, l.get_bounds().size.y / 100.f});
            window.draw(l_spr);
        }
    } else {
        sf::RectangleShape ladder_shape({16, 120});
        ladder_shape.setFillColor(sf::Color{180, 180, 180});
        for (auto& l : ladders) {
            ladder_shape.setPosition({l.get_bounds().position.x, l.get_bounds().position.y});
            ladder_shape.setSize({16, l.get_bounds().size.y});
            window.draw(ladder_shape);
        }
    }

    dk.draw(window);
    if (state.stage == 9) {
        if (princess_tex.getSize().x > 0) {
            sf::Sprite spr(princess_tex);
            spr.setPosition({70, 104});
            window.draw(spr);
        } else
            window.draw(princess);
    }
    player.draw(window);

    for (auto& b : barrels)
        b->draw(window);

    // HeartPickup
    if (heart_pickup) heart_pickup->draw(window);

    // PowerUp
    if (powerup) powerup->draw(window);

    // Pillar enemies
    if (left_pillar) left_pillar->draw(window);
    if (right_pillar) right_pillar->draw(window);

    // ── STAGE 1 TUTORIAL HINTS ──
    if (state.phase == GameState::Phase::Playing && state.stage == 1) {
        // Arrow pointing up toward the boss at the top
        {
            float ax = 90.f, ay = 300.f;
            sf::RectangleShape stem({4, 90});
            stem.setFillColor(sf::Color{255, 220, 80, 220});
            stem.setPosition({ax - 2, ay - 70});
            window.draw(stem);
            sf::ConvexShape head(3);
            head.setPoint(0, {ax - 14, ay - 70});
            head.setPoint(1, {ax + 14, ay - 70});
            head.setPoint(2, {ax, ay - 110});
            head.setFillColor(sf::Color{255, 220, 80, 220});
            window.draw(head);
        }

        auto hint_panel = [&](float x, float y, const std::string& title, const std::string& body) {
            sf::Text tt(font, title, 18);
            tt.setFillColor(sf::Color{255, 220, 80});
            auto ttb = tt.getLocalBounds();
            float panel_w = 260.f;
            sf::RectangleShape panel({panel_w, 74});
            panel.setFillColor(sf::Color{0, 0, 0, 160});
            panel.setOutlineColor(sf::Color{255, 220, 80});
            panel.setOutlineThickness(1.f);
            panel.setPosition({x, y});
            window.draw(panel);
            tt.setPosition({x + 12, y + 8});
            window.draw(tt);
            sf::Text bt(font, body, 14);
            bt.setFillColor(sf::Color::White);
            bt.setPosition({x + 12, y + 34});
            window.draw(bt);
        };

        hint_panel(120, 590, "MOVE & JUMP", "WASD or Arrow keys  |  SPACE = jump\nClimb ladders with UP / DOWN");
        hint_panel(420, 590, "DOUBLE JUMP", "Press SPACE again in the air\nJump across gaps and barrels");

        // Reach-the-boss hint below the arrow
        sf::Text reach(font, "REACH THE BOSS!", 18);
        reach.setFillColor(sf::Color{255, 220, 80});
        auto rb = reach.getLocalBounds();
        reach.setOrigin({rb.size.x / 2, 0});
        reach.setPosition({90, 330});
        window.draw(reach);

        // God-mode pickup marking (fixed location, middle of spawn platform)
        if (powerup && powerup->is_active()) {
            auto pp = powerup->get_pos();
            sf::Text god(font, "GOD MODE", 16);
            god.setFillColor(sf::Color{100, 255, 255});
            auto gb = god.getLocalBounds();
            god.setOrigin({gb.size.x / 2, 0});
            god.setPosition({pp.x + 12.f, pp.y - 90.f});
            window.draw(god);
            sf::RectangleShape stem({3, 55});
            stem.setFillColor(sf::Color{100, 255, 255, 220});
            stem.setPosition({pp.x + 12.f - 1.5f, pp.y - 75.f});
            window.draw(stem);
            sf::ConvexShape head(3);
            head.setPoint(0, {pp.x + 12.f - 12, pp.y - 20});
            head.setPoint(1, {pp.x + 12.f + 12, pp.y - 20});
            head.setPoint(2, {pp.x + 12.f, pp.y - 4});
            head.setFillColor(sf::Color{100, 255, 255, 220});
            window.draw(head);
        }
    }

    // Stage number (top-right)
    std::string sstr = std::to_string(state.stage);
    crowns_text.setString(sstr);
    crowns_text.setOrigin({crowns_text.getLocalBounds().size.x, 0});
    crowns_text.setPosition({770, 10});
    window.draw(crowns_text);

    // Mute button
    window.draw(mute_btn);
    window.draw(mute_text);

    // Lives display at bottom-center
    {
        int max_lives = 3;
        float spacing = 22.f;
        float start_x = 400 - (max_lives - 1) * spacing * 0.5f;
        float hy = 725.f;
        for (int i = 0; i < max_lives; i++) {
            float hx = start_x + i * spacing;
            sf::Color hc = i < state.lives ? sf::Color::Red : sf::Color{60, 60, 60};
            sf::CircleShape c(4.f);
            c.setOrigin({4, 4});
            c.setFillColor(hc);
            c.setOutlineColor(sf::Color::Black);
            c.setOutlineThickness(1.f);
            c.setPosition({hx - 4, hy - 2});
            window.draw(c);
            c.setPosition({hx + 4, hy - 2});
            window.draw(c);
            sf::ConvexShape tri(3);
            tri.setPoint(0, {hx - 5, hy + 1});
            tri.setPoint(1, {hx + 5, hy + 1});
            tri.setPoint(2, {hx, hy + 7});
            tri.setFillColor(hc);
            tri.setOutlineColor(sf::Color::Black);
            tri.setOutlineThickness(1.f);
            window.draw(tri);
        }
    }

    // Timer display (middle-top)
    {
        time_text.setString("OVERALL  " + GameState::fmt_time(state.overall_timer));
        auto ot = time_text.getLocalBounds();
        time_text.setOrigin({ot.size.x / 2, 0});
        time_text.setPosition({400, 8});
        window.draw(time_text);

        float orec = state.records.overall_time;
        if (orec >= 0) {
            overall_rec_text.setString("RECORD   " + GameState::fmt_time(orec));
            auto ort = overall_rec_text.getLocalBounds();
            overall_rec_text.setOrigin({ort.size.x / 2, 0});
            overall_rec_text.setPosition({400, 24});
            window.draw(overall_rec_text);
        }

        record_text.setString("STAGE    " + GameState::fmt_time(state.stage_timer));
        auto st = record_text.getLocalBounds();
        record_text.setOrigin({st.size.x / 2, 0});
        record_text.setPosition({400, 44});
        window.draw(record_text);

        float srec = state.records.stage_times[state.stage - 1];
        if (srec >= 0) {
            stage_rec_text.setString("RECORD   " + GameState::fmt_time(srec));
            auto srt = stage_rec_text.getLocalBounds();
            stage_rec_text.setOrigin({srt.size.x / 2, 0});
            stage_rec_text.setPosition({400, 60});
            window.draw(stage_rec_text);
        }
    }

    // ── PAUSED OVERLAY ──
    if (state.paused) {
        sf::RectangleShape overlay({800, 750});
        overlay.setFillColor(sf::Color{0, 0, 0, 150});
        window.draw(overlay);

        status_text.setString("PAUSED");
        auto stp = status_text.getLocalBounds();
        status_text.setOrigin({stp.size.x / 2, stp.size.y / 2});
        status_text.setPosition({400, 180});
        window.draw(status_text);

        pause_resume_btn.setSize({160, 50});
        pause_resume_btn.setOrigin({80, 25});
        pause_resume_btn.setPosition({400, 260});
        pause_resume_text.setCharacterSize(24);
        auto prb = pause_resume_text.getLocalBounds();
        pause_resume_text.setOrigin({prb.position.x + prb.size.x / 2, prb.position.y + prb.size.y / 2});
        pause_resume_text.setPosition({400, 260});
        window.draw(pause_resume_btn);
        window.draw(pause_resume_text);

        pause_reset_btn.setSize({160, 50});
        pause_reset_btn.setOrigin({80, 25});
        pause_reset_btn.setPosition({400, 340});
        pause_reset_text.setCharacterSize(24);
        auto prv = pause_reset_text.getLocalBounds();
        pause_reset_text.setOrigin({prv.position.x + prv.size.x / 2, prv.position.y + prv.size.y / 2});
        pause_reset_text.setPosition({400, 340});
        window.draw(pause_reset_btn);
        window.draw(pause_reset_text);
    }

    // ── GAME OVER OVERLAY ──
    if (state.phase == GameState::Phase::GameOver) {
        sf::RectangleShape overlay({800, 750});
        overlay.setFillColor(sf::Color{0, 0, 0, 150});
        window.draw(overlay);
        status_text.setString("GAME OVER");
        auto stg = status_text.getLocalBounds();
        status_text.setOrigin({stg.size.x / 2, stg.size.y / 2});
        status_text.setPosition({400, 250});
        window.draw(status_text);
        menu_btn.setPosition({400, 340});
        menu_btn_text.setString("MENU");
        auto mt = menu_btn_text.getLocalBounds();
        menu_btn_text.setOrigin({mt.position.x + mt.size.x / 2, mt.position.y + mt.size.y / 2});
        menu_btn_text.setPosition({400, 340});
        window.draw(menu_btn);
        window.draw(menu_btn_text);
    }

    // ── STAGE COMPLETE / WON OVERLAY ──
    if (state.phase == GameState::Phase::Won) {
        if (state.crowns >= 9) {
            // ── GRAND CONGRATULATIONS — final boss (stage 9) defeated ──
            sf::RectangleShape overlay({800, 750});
            overlay.setFillColor(sf::Color{0, 0, 0, 160});
            window.draw(overlay);

            // Celebrate: row of crowns across the top
            for (int c = 0; c < 9; c++) {
                sf::CircleShape star(8.f);
                star.setFillColor(c == 8 ? sf::Color{255, 220, 60} : sf::Color{230, 190, 40});
                star.setPosition({400 + (c - 4) * 60.f, 90 + (c % 2) * 10.f});
                window.draw(star);
            }

            status_text.setString("GRAND CONGRATULATIONS!");
            status_text.setCharacterSize(42);
            auto stb2 = status_text.getLocalBounds();
            status_text.setOrigin({stb2.size.x / 2, 0});
            status_text.setPosition({400, 150});
            status_text.setFillColor(sf::Color::Yellow);
            window.draw(status_text);

            sf::Text sub(font);
            sub.setString("You defeated the final boss!");
            sub.setCharacterSize(26);
            sub.setFillColor(sf::Color{255, 220, 100});
            auto subb = sub.getLocalBounds();
            sub.setOrigin({subb.size.x / 2, 0});
            sub.setPosition({400, 215});
            window.draw(sub);

            // Final boss sprite, scaled up
            if (boss_tex.getSize().x > 0) {
                sf::Sprite boss_spr(boss_tex);
                auto bs = boss_tex.getSize();
                float bsc = 90.f / (float)(bs.x > 0 ? bs.x : 90);
                boss_spr.setScale({bsc, bsc});
                boss_spr.setOrigin({(float)bs.x / 2.f, (float)bs.y / 2.f});
                boss_spr.setPosition({400, 300});
                window.draw(boss_spr);
            }

            bool new_overall_rec = (state.overall_timer < state.records.overall_time || state.records.overall_time < 0);
            std::string ov_line = "FINAL TIME: " + GameState::fmt_time(state.overall_timer);
            if (new_overall_rec) ov_line += "   NEW RECORD!";
            sf::Text ov_text(font);
            ov_text.setString(ov_line);
            ov_text.setCharacterSize(20);
            ov_text.setFillColor(new_overall_rec ? sf::Color::Yellow : sf::Color::White);
            auto ovt = ov_text.getLocalBounds();
            ov_text.setOrigin({ovt.size.x / 2, 0});
            ov_text.setPosition({400, 350});
            window.draw(ov_text);

            if (state.records.overall_time >= 0 && !new_overall_rec) {
                sf::Text prev_ov(font);
                prev_ov.setString("BEST: " + GameState::fmt_time(state.records.overall_time));
                prev_ov.setCharacterSize(18);
                prev_ov.setFillColor(sf::Color{180, 180, 200});
                auto povt = prev_ov.getLocalBounds();
                prev_ov.setOrigin({povt.size.x / 2, 0});
                prev_ov.setPosition({400, 375});
                window.draw(prev_ov);
            }

            // Buttons: PLAY AGAIN (menu_btn) + MENU (pause_reset_btn), same hitboxes as usual
            float bx[] = {300, 500};
            const char* labels[] = {"PLAY AGAIN", "MENU"};
            sf::RectangleShape* btns[] = {&menu_btn, &pause_reset_btn};
            sf::Text* btn_texts[] = {&menu_btn_text, &pause_reset_text};
            for (int bi = 0; bi < 2; bi++) {
                btns[bi]->setSize({160, 50});
                btns[bi]->setOrigin({80, 25});
                btns[bi]->setPosition({bx[bi], 430});
                btns[bi]->setFillColor(sf::Color{60, 60, 180});
                window.draw(*btns[bi]);
                btn_texts[bi]->setString(labels[bi]);
                btn_texts[bi]->setCharacterSize(24);
                btn_texts[bi]->setFillColor(sf::Color::White);
                auto tb = btn_texts[bi]->getLocalBounds();
                btn_texts[bi]->setOrigin({tb.position.x + tb.size.x / 2, tb.position.y + tb.size.y / 2});
                btn_texts[bi]->setPosition({bx[bi], 430});
                window.draw(*btn_texts[bi]);
            }

            sf::Text skip_hint(font);
            skip_hint.setString("Press SPACE to play again");
            skip_hint.setCharacterSize(18);
            skip_hint.setFillColor(sf::Color{200, 200, 220});
            auto skh = skip_hint.getLocalBounds();
            skip_hint.setOrigin({skh.size.x / 2, 0});
            skip_hint.setPosition({400, 505});
            window.draw(skip_hint);
        } else {
        sf::RectangleShape overlay({800, 750});
        overlay.setFillColor(sf::Color{0, 0, 0, 150});
        window.draw(overlay);
        int si = state.stage - 1;
        float prev_stage = state.records.stage_times[si];
        bool new_stage_rec = (state.stage_timer < prev_stage || prev_stage < 0);

        status_text.setString("STAGE COMPLETE!");
        auto stb = status_text.getLocalBounds();
        status_text.setOrigin({stb.size.x / 2, 0});
        status_text.setPosition({400, 120});
        window.draw(status_text);

        sf::Text st_time(font);
        st_time.setCharacterSize(18);
        std::string st_line = "STAGE TIME:   " + GameState::fmt_time(state.stage_timer);
        if (new_stage_rec) st_line += "   NEW RECORD!";
        st_time.setString(st_line);
        st_time.setFillColor(new_stage_rec ? sf::Color::Yellow : sf::Color::White);
        auto stt = st_time.getLocalBounds();
        st_time.setOrigin({stt.size.x / 2, 0});
        st_time.setPosition({400, 180});
        window.draw(st_time);

        if (prev_stage >= 0) {
            sf::Text prev_st(font);
            prev_st.setString("BEST:         " + GameState::fmt_time(prev_stage));
            prev_st.setCharacterSize(18);
            prev_st.setFillColor(sf::Color{180, 180, 200});
            auto pst = prev_st.getLocalBounds();
            prev_st.setOrigin({pst.size.x / 2, 0});
            prev_st.setPosition({400, 205});
            window.draw(prev_st);
        }

        sf::Text ov_text(font);
            ov_text.setCharacterSize(18);
            ov_text.setString("OVERALL: " + GameState::fmt_time(state.overall_timer));
            ov_text.setFillColor(sf::Color::White);
            auto ovt = ov_text.getLocalBounds();
            ov_text.setOrigin({ovt.size.x / 2, 0});
            ov_text.setPosition({400, 240});
            window.draw(ov_text);

        std::string sstr2 = std::to_string(state.stage);
        crowns_text.setString(sstr2);
        crowns_text.setPosition({770, 10});
        crowns_text.setOrigin({crowns_text.getLocalBounds().size.x, 0});
        window.draw(crowns_text);

        float bx[] = {300, 500};
        const char* labels[] = {"NEXT STAGE", "MENU"};
        sf::RectangleShape* btns[] = {&menu_btn, &pause_reset_btn};
        sf::Text* btn_texts[] = {&menu_btn_text, &pause_reset_text};
        for (int bi = 0; bi < 2; bi++) {
            btns[bi]->setSize({160, 50});
            btns[bi]->setOrigin({80, 25});
            btns[bi]->setPosition({bx[bi], 340});
            btns[bi]->setFillColor(sf::Color{60, 60, 180});
            window.draw(*btns[bi]);
            btn_texts[bi]->setString(labels[bi]);
            btn_texts[bi]->setCharacterSize(24);
            btn_texts[bi]->setFillColor(sf::Color::White);
            auto tb = btn_texts[bi]->getLocalBounds();
            btn_texts[bi]->setOrigin({tb.position.x + tb.size.x / 2, tb.position.y + tb.size.y / 2});
            btn_texts[bi]->setPosition({bx[bi], 340});
            window.draw(*btn_texts[bi]);
        }

        sf::Text skip_hint(font);
        skip_hint.setString("Press SPACE to continue");
        skip_hint.setCharacterSize(18);
        skip_hint.setFillColor(sf::Color{200, 200, 220});
        auto skh = skip_hint.getLocalBounds();
        skip_hint.setOrigin({skh.size.x / 2, 0});
        skip_hint.setPosition({400, 405});
        window.draw(skip_hint);
        }
    }

    window.display();
}
