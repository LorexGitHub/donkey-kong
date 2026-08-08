#ifndef DK_HEARTPICKUP_H
#define DK_HEARTPICKUP_H

#include <SFML/Graphics.hpp>

class HeartPickup {
public:
    HeartPickup(float x, float y);
    void update(float dt);
    void draw(sf::RenderWindow& win) const;
    sf::FloatRect get_bounds() const;
    sf::Vector2f get_pos() const { return pos; }
    bool is_active() const { return active; }
    void collect() { active = false; }

private:
    sf::Vector2f pos;
    bool active = true;
    float anim_timer = 0;
    static constexpr float SIZE = 14.f;
};

#endif