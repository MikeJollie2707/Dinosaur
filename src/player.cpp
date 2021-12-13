#include "player.hpp"

Dinosaur::Dinosaur()
{
    vy        = 0.0;
    is_crouch = false;
    hitbox.setSize(sf::Vector2f(15, -30));
    hitbox.setFillColor(sf::Color(166, 172, 175));
    hitbox.setPosition(100, 150);
}

void Dinosaur::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(hitbox, states);
}

void Dinosaur::move(double dt)
{
    hitbox.move(0, -(vy * dt));
}

void Dinosaur::align_ground(const double& y_level)
{
    hitbox.setPosition(hitbox.getPosition().x, y_level);
}

void Dinosaur::set_crouch(bool crouch_status)
{
    is_crouch = crouch_status;
    if (is_crouch) {
        hitbox.setSize(sf::Vector2f(30, -15));
    }
    else {
        hitbox.setSize(sf::Vector2f(15, -30));
    }
}
