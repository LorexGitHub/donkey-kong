#ifndef DK_PICKUP_H
#define DK_PICKUP_H

#include <SFML/Graphics.hpp>

/// Base class for collectible items: shared position, visibility, animation state.
/// Subclasses define their own geometry (get_bounds) and look (draw).
class Pickup {
public:
    Pickup(float x, float y, float anim_rate)
        : pos(x, y), anim_rate(anim_rate) {}
    virtual ~Pickup() = default;

    void update(float dt) { if (active) anim_timer += dt * anim_rate; }
    virtual void draw(sf::RenderWindow& win) const = 0;
    virtual sf::FloatRect get_bounds() const = 0;

    sf::Vector2f get_pos() const { return pos; }
    bool is_active() const { return active; }
    void collect() { active = false; }

protected:
    sf::Vector2f pos;
    float anim_rate;
    float anim_timer = 0;
    bool active = true;
};

#endif