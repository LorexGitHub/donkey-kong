#include "PillarEnemy.hpp"
#include <cstdlib>
#include <cmath>

PillarEnemy::PillarEnemy(float x, float y_min, float y_max, int dir_sign)
    : pos{x, y_min}, y_min(y_min), y_max(y_max), dir_sign(dir_sign) {
    shape.setOrigin({8.f, 8.f});
    shape.setFillColor(sf::Color{220, 100, 20});
    shape.setPosition(pos);
    // Randomize the first shot so enemies don't fire in sync.
    fire_timer = static_cast<float>(std::rand() % 100) / 100.f * fire_interval;
}

void PillarEnemy::update(float dt) {
    // Patrol: bounce between the two track bounds.
    pos.y += dir * speed * dt;
    if (pos.y < y_min) { pos.y = y_min; dir = 1; }
    if (pos.y > y_max) { pos.y = y_max; dir = -1; }
    shape.setPosition(pos);

    anim_timer += dt;

    // Spawn a fireball toward the player's side every fire_interval seconds.
    fire_timer += dt;
    if (fire_timer >= fire_interval) {
        fire_timer = 0;
        Fireball fb;
        fb.pos = {pos.x, pos.y};
        fb.vel = {fireball_speed * dir_sign, 0.f};
        fb.alive = true;
        fireballs.push_back(fb);
    }

    // Move fireballs; ones leaving the stage are removed.
    for (auto it = fireballs.begin(); it != fireballs.end();) {
        it->pos += it->vel * dt;
        if (it->pos.x < -50 || it->pos.x > 850)
            it->alive = false;
        if (!it->alive)
            it = fireballs.erase(it);
        else
            ++it;
    }
}

void PillarEnemy::draw(sf::RenderWindow& win) const {
    win.draw(shape);

    // Fireballs pulse orange with a yellow outline.
    for (auto& fb : fireballs) {
        sf::CircleShape ball(5.f);
        ball.setOrigin({5.f, 5.f});
        ball.setPosition(fb.pos);
        float pulse = 1.f + std::sin(anim_timer * 8.f) * 0.15f;
        ball.setFillColor(sf::Color(
            static_cast<std::uint8_t>(255),
            static_cast<std::uint8_t>(150 * pulse),
            30));
        ball.setOutlineColor(sf::Color::Yellow);
        ball.setOutlineThickness(1.f);
        win.draw(ball);
    }
}

sf::FloatRect PillarEnemy::get_bounds() const {
    return {{pos.x - 8.f, pos.y - 8.f}, {16.f, 16.f}};
}