#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>

class Dinosaur : public sf::Drawable {
private:
    sf::RectangleShape hitbox;
    double             vy;
    bool               is_crouch;

public:
    double        get_velocity() const { return vy; }
    sf::Vector2f  get_hitbox_position() const { return hitbox.getPosition(); }
    sf::FloatRect get_hitbox_shape() const { return hitbox.getGlobalBounds(); }


    Dinosaur& set_velocity(double new_vy) { vy = new_vy; return *this; }
    void move(double dt);
    void align_ground(const double& y_level);
    void set_crouch(bool crouch_status);
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    Dinosaur();
};

#endif