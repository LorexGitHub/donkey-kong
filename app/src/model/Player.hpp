#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>

/// The player character: position, velocity, jump/climb state and animation.
/// Movement is driven by semantic commands (move/jump/climb) issued by the
/// GameController, so the class never sees concrete keyboard input.
class Player {
public:
    Player();

    // -- Input commands (issued by the controller) --
    void move_left();      ///< Start moving left (cancels climbing).
    void move_right();     ///< Start moving right (cancels climbing).
    void stop_horizontal();///< Stop horizontal movement.
    void jump();           ///< Jump (double jump supported); resets on ground contact.
    void climb(float speed);      ///< Climb a ladder at the given vertical speed.
    void stop_on_ladder();        ///< Stand still while attached to a ladder.

    /// Advance the simulation by dt seconds: gravity, movement, animation.
    void update(float dt);

    void set_position(float x, float y);  ///< Teleport to (x, y) and reset velocity.
    void clamp_x(float lo, float hi);     ///< Keep the player inside the stage bounds.
    void bump_head(float y);              ///< Stop an upward climb at a ceiling.
    void set_on_ground(bool g);           ///< Grounded players regain their jumps.
    void set_dead(bool d) { dead = d; }
    void draw(sf::RenderWindow& w) const; ///< Draw sprite (or shape fallback).
    void set_invincible(float duration) { invincible = true; invincible_timer = duration; }
    bool is_invincible() const { return invincible; }
    float get_invincible_timer() const { return invincible_timer; }

    // -- State queries (used by the view and collision handling) --
    sf::FloatRect get_bounds() const;
    sf::Vector2f get_pos() const { return pos; }
    sf::Vector2f get_vel() const { return vel; }
    bool is_on_ground() const { return on_ground; }
    bool is_climbing() const { return climbing; }
    void set_climbing(bool c) { climbing = c; }
    int get_jumps_left() const { return jumps_left; }

private:
    /// Movement speed, boosted per consecutive air jump (bunny hop).
    float get_current_speed() const;

    sf::Vector2f pos;
    sf::Vector2f vel{0, 0};
    sf::RectangleShape shape{sf::Vector2f{24, 32}};
    sf::Texture texture;
    float anim_timer = 0;
    int anim_frame = 0;
    bool on_ground = false;
    bool climbing = false;
    bool dead = false;
    int dir = 0;
    int jumps_left = 2;
    int bunny_count = 0;
    bool invincible = false;
    float invincible_timer = 0;

    static constexpr float SPEED = 200.f;   ///< Base horizontal speed (px/s).
    static constexpr float JUMP_VEL = -330.f; ///< Upward jump velocity (px/s).
    static constexpr float GRAVITY = 900.f; ///< Downward acceleration (px/s^2).
    static constexpr float AIR_ACCEL = 15.f;  ///< Air-control lerp factor per second.
    static constexpr int MAX_JUMPS = 2;       ///< One ground jump + one air jump.
    static constexpr float BUNNY_BOOST = 0.25f; ///< Speed bonus per air jump.
};

#endif