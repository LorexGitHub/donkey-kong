#ifndef DK_POWERUP_H
#define DK_POWERUP_H

#include "Pickup.hpp"

class PowerUp : public Pickup {
public:
    PowerUp(float x, float y) : Pickup(x, y, 4.f) {}
    void draw(sf::RenderWindow& win) const override;
    sf::FloatRect get_bounds() const override;
private:
    static constexpr float SIZE = 10.f;
};

#endif