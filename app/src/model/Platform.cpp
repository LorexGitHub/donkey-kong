#include "Platform.hpp"

Platform::Platform(float x, float y, float w) : shape(sf::Vector2f{w, 14}) {
    shape.setPosition({x, y});
    shape.setFillColor(sf::Color{139, 69, 19}); // brown
}

sf::FloatRect Platform::get_bounds() const {
    return shape.getGlobalBounds();
}

bool Platform::solid_segment_hit(const sf::FloatRect& r) const {
    const float SEG_W = 70.f;
    auto pb = get_bounds();
    for (int s = 0; s < 10; s++) {
        if (!is_solid(s)) continue;
        sf::FloatRect seg = {{pb.position.x + s * SEG_W, pb.position.y}, {SEG_W, 14.f}};
        if (r.findIntersection(seg).has_value())
            return true;
    }
    return false;
}
