#ifndef DK_HEARTPICKUP_H
#define DK_HEARTPICKUP_H

#include "Pickup.hpp"

class HeartPickup : public Pickup {
public:
    HeartPickup(float x, float y) : Pickup(x, y, 3.f) {}
    void draw(sf::RenderWindow& win) const override;
    sf::FloatRect get_bounds() const override;
private:
    static constexpr float SIZE = 14.f;
};

#endif