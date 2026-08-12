#include "HeartPickup.hpp"
#include <cmath>

HeartPickup::HeartPickup(float x, float y) : Pickup(x, y, 3.f) {}

void HeartPickup::draw(sf::RenderWindow& win) const {
    if (!is_active()) return;

    float pulse = 1.f + std::sin(anim_timer) * 0.15f;
    float s = SIZE * pulse;

    // Left lobe
    sf::CircleShape l(s * 0.45f);
    l.setOrigin({s * 0.45f, s * 0.45f});
    l.setPosition({pos.x - s * 0.5f, pos.y - s * 0.35f});
    l.setFillColor(sf::Color(220, 40, 60));
    win.draw(l);

    // Right lobe
    sf::CircleShape r(s * 0.45f);
    r.setOrigin({s * 0.45f, s * 0.45f});
    r.setPosition({pos.x + s * 0.5f, pos.y - s * 0.35f});
    r.setFillColor(sf::Color(220, 40, 60));
    win.draw(r);

    // Bottom triangle point
    sf::ConvexShape tri(3);
    tri.setPoint(0, {pos.x - s * 0.55f, pos.y - s * 0.15f});
    tri.setPoint(1, {pos.x + s * 0.55f, pos.y - s * 0.15f});
    tri.setPoint(2, {pos.x, pos.y + s * 0.7f});
    tri.setFillColor(sf::Color(220, 40, 60));
    win.draw(tri);

    // White highlight
    sf::CircleShape highlight(s * 0.18f);
    highlight.setFillColor(sf::Color(255, 220, 220));
    highlight.setOrigin({s * 0.18f, s * 0.18f});
    highlight.setPosition({pos.x - s * 0.25f, pos.y - s * 0.35f});
    win.draw(highlight);
}

sf::FloatRect HeartPickup::get_bounds() const {
    float s = SIZE * 1.2f;
    return {{pos.x - s, pos.y - s}, {s * 2, s * 2}};
}