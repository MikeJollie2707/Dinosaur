#ifndef DINOSAUR_INCLUDE_GAME_HPP
#define DINOSAUR_INCLUDE_GAME_HPP

#include "obstacle.hpp"
#include "player.hpp"
#include "resource_manager.hpp"

#include <SFML/Audio.hpp>

#include <cmath>
#include <vector>

class Game {
private:
    const double c_gravity        = 400;
    const double c_max_height     = 69;
    const double c_ground_y_level = 150;
    const double c_score_interval = 0.05;  // Seconds

    const double c_init_obstacle_speed   = 70.0;
    const double c_init_obstacle_spacing = 150.0;
    const double c_init_spawn            = 300.0;

    const sf::Color c_light_bg = sf::Color(229, 231, 233);
    const sf::Color c_dark_bg  = sf::Color(46, 64, 83);

    Dinosaur              dino;
    std::vector<Obstacle> obstacles;

    int    score;
    int    obstacle_difficulty;
    double obstacle_speed;
    double obstacle_spacing;
    double init_spawn;
    bool   is_pause;
    bool   is_lose;
    bool   is_jumping;
    bool   is_falling;

    sf::Clock      clock;
    const sf::Time time_per_frame;
    sf::Clock      score_timer;

    enum class Sound { Jump,
                       Score };
    enum class Font { Consolas };

    ResourceManager<sf::Font, Font>         font;
    ResourceManager<sf::SoundBuffer, Sound> sounds;
    sf::Sound                               jump_sound;
    sf::Sound                               score_sound;
    sf::RectangleShape                      right_pad;
    sf::RectangleShape                      left_pad;
    sf::RectangleShape                      ground_pad;
    sf::Text                                pause_text;
    sf::Text                                score_text;

    sf::RenderWindow window;
    sf::Color        background_color;

    double    get_init_jump_velocity(double max_height) const { return std::sqrt(2 * c_gravity * max_height); }
    double    get_obstacle_speed(int score, double current_speed) const;
    double    get_obstacle_spacing(int score, double current_spacing) const;
    sf::Color get_opposite_theme(sf::Color theme) const;

    void process_events();
    void update(double dt);
    void render();

public:
    Game(double logic_frame = 1.0 / 60.0);
    void run();
};

#endif