#ifndef DK_PILLARENEMY_H
#define DK_PILLARENEMY_H

#include <SFML/Graphics.hpp>
#include <vector>

class PillarEnemy {
public:
    PillarEnemy(float x, float y_min, float y_max, int dir_sign);

    void update(float dt);
    void draw(sf::RenderWindow& win) const;

    sf::FloatRect get_bounds() const;
    sf::Vector2f get_pos() const { return pos; }

    void set_move_speed(float s) { speed = s; }
    void set_fire_interval(float i) { fire_interval = i; }
    void set_fireball_speed(float s) { fireball_speed = s; }

    struct Fireball {
        sf::Vector2f pos;
        sf::Vector2f vel;
        bool alive = true;
    };
    std::vector<Fireball>& get_fireballs() { return fireballs; }

private:
    sf::Vector2f pos;
    float y_min, y_max;
    int dir_sign;
    int dir = 1;
    float speed = 100.f;
    float fire_timer = 0;
    float fire_interval = 2.5f;
    float anim_timer = 0;
    float fireball_speed = 250.f;
    sf::CircleShape shape{8.f};
    std::vector<Fireball> fireballs;
};

#endif
