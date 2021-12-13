#include "obstacle.hpp"

Obstacle_Type getRandomObstacleType(int upper_limit)
{
    if (upper_limit > 6)
        upper_limit = 6;
    static std::random_device             rd;
    static std::mt19937                   gen(rd());
    std::uniform_int_distribution<size_t> dis(0, upper_limit);
    return static_cast<Obstacle_Type>(dis(gen));
}

Obstacle::Obstacle(const Obstacle_Type& type, double init_x, double velocity)
{
    switch (type) {
    case Obstacle_Type::Cactus1High: {
        hitbox.setSize(sf::Vector2f(10, -25));
        hitbox.setFillColor(sf::Color(34, 153, 84));
        hitbox.setPosition(init_x, 150);
        break;
    }
    case Obstacle_Type::Cactus1Low: {
        hitbox.setSize(sf::Vector2f(10, -20));
        hitbox.setFillColor(sf::Color(34, 153, 84));
        hitbox.setPosition(init_x, 150);
        break;
    }
    case Obstacle_Type::Cactus2: {
        hitbox.setSize(sf::Vector2f(20, -25));
        hitbox.setFillColor(sf::Color(34, 153, 84));
        hitbox.setPosition(init_x, 150);
        break;
    }
    case Obstacle_Type::CactusN: {
        hitbox.setSize(sf::Vector2f(35, -25));
        hitbox.setFillColor(sf::Color(34, 153, 84));
        hitbox.setPosition(init_x, 150);
        break;
    }
    case Obstacle_Type::BirdHigh: {
        hitbox.setSize(sf::Vector2f(15, -10));
        hitbox.setFillColor(sf::Color(235, 152, 78));
        hitbox.setPosition(init_x, 105);
        break;
    }
    case Obstacle_Type::BirdMid: {
        hitbox.setSize(sf::Vector2f(15, -10));
        hitbox.setFillColor(sf::Color(235, 152, 78));
        hitbox.setPosition(init_x, 125);
        break;
    }
    case Obstacle_Type::BirdLow: {
        hitbox.setSize(sf::Vector2f(15, -10));
        hitbox.setFillColor(sf::Color(235, 152, 78));
        hitbox.setPosition(init_x, 145);
        break;
    }
    }

    this->velocity = velocity;
}

void Obstacle::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(hitbox, states);
}

Obstacle& Obstacle::set_x_position(double x)
{
    hitbox.setPosition(x, hitbox.getPosition().y);
    return *this;
}

Obstacle& Obstacle::set_velocity(double velocity)
{
    this->velocity = velocity;
    return *this;
}

void Obstacle::move(double dt)
{
    hitbox.move(sf::Vector2f(dt * -velocity, 0));
}
