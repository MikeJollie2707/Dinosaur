#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP

#include <SFML/Graphics.hpp>
#include <random>

enum class Obstacle_Type {
    Cactus1High,
    Cactus1Low,
    Cactus2,
    CactusN,
    BirdHigh,
    BirdMid,
    BirdLow
};

Obstacle_Type getRandomObstacleType(int upper_limit = 6);

class Obstacle : public sf::Drawable {
private:
    double             velocity;
    sf::RectangleShape hitbox;

public:
    sf::FloatRect get_hitbox_shape() const { return hitbox.getGlobalBounds(); }
    sf::Vector2f  get_position() const { return hitbox.getPosition(); }

    Obstacle& set_x_position(double x);
    Obstacle& set_velocity(double velocity);
    void      move(double dt);
    void      draw(sf::RenderTarget& target, sf::RenderStates states) const;

    Obstacle(const Obstacle_Type& type, double init_x, double velocity = 70.0);
};

#endif