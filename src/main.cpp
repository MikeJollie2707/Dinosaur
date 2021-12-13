#include "obstacle.hpp"
#include "player.hpp"

#include <SFML/Audio.hpp>

#include <cmath>
#include <iostream>
#include <vector>

const double c_gravity        = 400;
const double c_max_height     = 69;
const double c_ground_y_level = 150;
const double c_score_interval = 0.05;  // Seconds

const double c_init_obstacle_speed   = 70.0;
const double c_init_obstacle_spacing = 150.0;
const double c_init_spawn            = 300.0;

const sf::Color c_light_bg = sf::Color(229, 231, 233);
const sf::Color c_dark_bg  = sf::Color(46, 64, 83);

double getInitVelocity(double height)
{
    return std::sqrt(2 * c_gravity * height);
}

double getObstacleSpeed(int score, double current_speed)
{
    if (score <= 800) {
        current_speed += (score / 100) * 2;
    }
    else {
        // c_init_obstacle_speed + 36 * 2 is the theoretical limit of the above speed function.
        current_speed = c_init_obstacle_speed + 36 * 2 + (score / 100) * 4;

        // Hard upper limit.
        if (current_speed > 300)
            current_speed = 300;
    }

    return current_speed;
}

double getObstacleSpacing(int score, double current_spacing)
{
    if (score <= 800) {
        current_spacing += (score / 100) * 4;
    }
    else {
        // c_init_obstacle_spacing + 36 * 4 is the theoretical limit of the above space function.
        current_spacing = c_init_obstacle_spacing + 36 * 4 + (score / 100) * 8;

        // Hard upper limit.
        if (current_spacing > 300)
            current_spacing = 300;
    }

    return current_spacing;
}

int main()
{
    // Init

    sf::Font font;
    if (!font.loadFromFile("../assets/consola.ttf")) {
        std::cerr << "Can't find consola.ttf file.\n";
        return -1;
    }

    sf::SoundBuffer jump_soundbuffer;
    sf::Sound       jump_sound;
    if (!jump_soundbuffer.loadFromFile("../assets/jump.wav")) {
        std::cerr << "Can't find jump.wav audio file.\n";
    }
    else
        jump_sound.setBuffer(jump_soundbuffer);

    sf::SoundBuffer score_soundbuffer;
    sf::Sound       score_sound;
    if (!score_soundbuffer.loadFromFile("../assets/point.wav")) {
        std::cerr << "Can't find point.wav audio file.\n";
    }
    else
        score_sound.setBuffer(score_soundbuffer);

    std::cout << "Controls:\n";
    std::cout << "- Arrow Up/Spacebar: Jump\n";
    std::cout << "- Arrow Down: Crouch\n";
    std::cout << "- Esc: Pause/Resume\n";
    std::cout << "The game is pausing; press Esc to start.\n";

    sf::RenderWindow window(sf::VideoMode(550, 200), "Dinosaur");
    // Remove the key holding delay default by OS.
    window.setKeyRepeatEnabled(false);

    sf::Color background_color = c_light_bg;

    sf::RectangleShape left_pad;
    left_pad.setPosition(0, 0);
    left_pad.setSize(sf::Vector2f(20, window.getSize().y));

    sf::RectangleShape right_pad;
    right_pad.setPosition(350, 0);
    right_pad.setSize(sf::Vector2f(window.getSize().x - 350, window.getSize().y));

    sf::Color          ground_color = sf::Color::Black;
    sf::RectangleShape ground_pad;
    ground_pad.setPosition(20, c_ground_y_level);
    ground_pad.setSize(sf::Vector2f(330, 0.5));

    sf::Text  score_text("", font);
    sf::Color text_color = c_dark_bg;
    score_text.setCharacterSize(15);
    score_text.setPosition(400, 0);
    score_text.setFillColor(text_color);

    sf::Text pause_text("Paused", font);
    sf::Color pause_color = c_light_bg;
    pause_text.setCharacterSize(20);
    pause_text.setPosition(400, 50);
    pause_text.setFillColor(pause_color);

    Dinosaur              dino;
    int                   obstacle_difficulty = 2;
    double                obstacle_spacing    = c_init_obstacle_spacing;
    double                obstacle_speed      = c_init_obstacle_speed;
    double                init_spawn          = c_init_spawn;
    std::vector<Obstacle> obstacles;
    for (int i = 0; i < 3; ++i) {
        obstacles.emplace_back(getRandomObstacleType(obstacle_difficulty), init_spawn, obstacle_speed);
        init_spawn += obstacle_spacing;
    }

    sf::Clock clock;
    sf::Clock score_timer;
    int       score        = 0;
    bool      is_pause     = true;
    bool      is_lose      = false;
    bool      is_jump_held = false;
    bool      is_jumping   = false;
    bool      is_falling   = false;

    // Game loop

    while (window.isOpen()) {
        sf::Time time = sf::Time::Zero;

        // Event handling

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space || event.key.code == sf::Keyboard::Up) {
                    // If dino is not falling then it's a fake jump.
                    // If dino is jumping then this doesn't affect anything.
                    if (!is_falling) {
                        is_jump_held = true;
                        // Force remove crouch.
                        dino.set_crouch(false);
                    }
                    std::cout << "Pressed Jump\n";
                }
                else if (event.key.code == sf::Keyboard::Down) {
                    // Can't crouch midair.
                    // Player can't change crouch status during pause either.
                    if (!is_jumping && !is_falling && !is_pause) {
                        dino.set_crouch(true);
                    }
                    std::cout << "Pressed Down\n";
                }
                else if (event.key.code == sf::Keyboard::Escape) {
                    // Bug: If the player holds crouch then pause, then release it, the dino will crouch until next jump/crouch toggle.
                    is_pause = !is_pause;
                    std::cout << "Pressed Esc\n";
                }
            }
            else if (event.type == sf::Event::KeyReleased) {
                if (event.key.code == sf::Keyboard::Space || event.key.code == sf::Keyboard::Up) {
                    if (!is_falling) {
                        // Super Meat Boy jump cancellation
                        dino.set_velocity(0);
                    }
                    is_jump_held = false;
                    std::cout << "Released Jump\n";
                }
                else if (event.key.code == sf::Keyboard::Down) {
                    if (!is_jumping && !is_falling && !is_pause) {
                        dino.set_crouch(false);
                    }
                }
            }
        }

        // Update

        // Pause handling. Time is 0 if the game is paused.
        // Also handle score incrementing.
        // Unhandled Situation: pausing will make scoring not work accurately.
        if (!is_pause && !is_lose) {
            time = clock.restart();

            if (score_timer.getElapsedTime().asSeconds() > c_score_interval) {
                ++score;
                score_text.setString(std::to_string(score));
                
                if (score % 100 == 0)
                    score_sound.play();
                
                score_timer.restart();
            }
            pause_color = background_color;
        }
        else {
            if (background_color == c_light_bg) {
                pause_color = c_dark_bg;
            }
            else {
                pause_color = c_light_bg;
            }
            clock.restart();
        }

        // Jump physics
        if (is_jump_held) {
            if (!is_jumping && !is_falling) {
                dino.set_velocity(getInitVelocity(c_max_height));
                is_jumping = true;
                jump_sound.play();
            }
        }

        dino.set_velocity(dino.get_velocity() - c_gravity * time.asSeconds());
        dino.move(time.asSeconds());
        if (is_jumping && dino.get_velocity() <= 0) {
            is_falling = true;
            is_jumping = false;
        }

        // Obstacle collision
        if (dino.get_hitbox_shape().intersects(obstacles.front().get_hitbox_shape())) {
            is_lose = true;
            pause_text.setString("Game Over");
        }

        // Ground collision
        if (dino.get_hitbox_position().y > c_ground_y_level) {
            dino.align_ground(c_ground_y_level);
            dino.set_velocity(0);
            is_falling = false;
        }

        // Generate new obstacle
        // Since obstacle generation also relates to
        if (obstacles.front().get_position().x < -40) {
            obstacles.erase(obstacles.begin());
            Obstacle& last_obstacle = obstacles.back();

            // Difficulty scaling
            if (score >= 200)
                obstacle_difficulty = 3;
            if (score >= 500)
                obstacle_difficulty = 6;

            obstacles.emplace_back(getRandomObstacleType(obstacle_difficulty), last_obstacle.get_position().x + obstacle_spacing, obstacle_speed);
        }

        if (score % 100 == 0) {
            static int pre_score = 0;
            if (pre_score != score) {
                pre_score = score;
                obstacle_speed   = getObstacleSpeed(score, obstacle_speed);
                obstacle_spacing = getObstacleSpacing(score, obstacle_spacing);
            }
        }

        for (int i = 0; i < obstacles.size(); ++i) {
            obstacles[i].set_velocity(obstacle_speed);
            obstacles[i].move(time.asSeconds());
        }

        // Change theme every 1000 points.
        if (score % 1000 == 0 && score != 0) {
            // Since this branch is repeated multiple times before score is updated, we use this to prevent swapping color many times.
            static int pre_score = 0;
            if (pre_score != score) {
                pre_score = score;
                if (background_color == c_light_bg)
                    background_color = c_dark_bg;
                else
                    background_color = c_light_bg;
                if (ground_color == sf::Color::Black)
                    ground_color = sf::Color::White;
                else
                    ground_color = sf::Color::Black;
                if (text_color == c_dark_bg)
                    text_color = c_light_bg;
                else
                    text_color = c_dark_bg;
                if (pause_color == c_light_bg)
                    pause_color = c_dark_bg;
                else
                    pause_color = c_light_bg;
            }
        }

        // Render

        window.clear(background_color);

        window.draw(dino);
        for (int i = 0; i < obstacles.size(); ++i) {
            window.draw(obstacles[i]);
        }

        left_pad.setFillColor(background_color);
        window.draw(left_pad);
        right_pad.setFillColor(background_color);
        window.draw(right_pad);
        ground_pad.setFillColor(ground_color);
        window.draw(ground_pad);
        score_text.setFillColor(text_color);
        window.draw(score_text);
        pause_text.setFillColor(pause_color);
        window.draw(pause_text);

        window.display();
    }

    return 0;
}