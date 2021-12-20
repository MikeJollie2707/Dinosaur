#include "game.hpp"

Game::Game(double logic_frame) :
    time_per_frame(sf::seconds(logic_frame > 0.0 ? logic_frame : 1.0 / 60.0))
{
    font.load(Font::Consolas, "../assets/consola.ttf");
    sounds.load(Sound::Jump, "../assets/jump.wav");
    sounds.load(Sound::Score, "../assets/point.wav");

    obstacle_difficulty = 2;
    obstacle_speed      = c_init_obstacle_speed;
    obstacle_spacing    = c_init_obstacle_spacing;
    init_spawn          = c_init_spawn;

    for (int i = 0; i < 3; ++i) {
        obstacles.emplace_back(getRandomObstacleType(obstacle_difficulty), init_spawn, obstacle_speed);
        init_spawn += obstacle_spacing;
    }

    score      = 0;
    is_pause   = true;
    is_lose    = false;
    is_jumping = false;
    is_falling = false;

    background_color = c_light_bg;

    window.create(sf::VideoMode(550, 200), "Dinosaur");
    window.setKeyRepeatEnabled(false);

    left_pad.setPosition(0, 0);
    left_pad.setSize(sf::Vector2f(20, window.getSize().y));
    left_pad.setFillColor(c_light_bg);

    right_pad.setPosition(350, 0);
    right_pad.setSize(sf::Vector2f(window.getSize().x - 350, window.getSize().y));
    right_pad.setFillColor(c_light_bg);

    ground_pad.setPosition(20, c_ground_y_level);
    ground_pad.setSize(sf::Vector2f(330, 0.5));
    ground_pad.setFillColor(c_dark_bg);

    score_text.setFont(font.get(Font::Consolas));
    score_text.setString("0");
    score_text.setPosition(400, 0);
    score_text.setCharacterSize(15);
    score_text.setFillColor(c_dark_bg);

    pause_text.setFont(font.get(Font::Consolas));
    pause_text.setString("Paused");
    pause_text.setPosition(400, 50);
    pause_text.setCharacterSize(20);
    pause_text.setFillColor(c_light_bg);

    jump_sound.setBuffer(sounds.get(Sound::Jump));
    score_sound.setBuffer(sounds.get(Sound::Score));
    score_sound.setVolume(15);

    clock.restart();
    score_timer.restart();
}

double Game::get_obstacle_speed(int score, double current_speed) const
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

double Game::get_obstacle_spacing(int score, double current_spacing) const
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

sf::Color Game::get_opposite_theme(sf::Color theme) const
{
    return theme == c_light_bg ? c_dark_bg : c_light_bg;
}

void Game::process_events()
{
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        else if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                is_pause = !is_pause;
            }
        }
        else if (event.type == sf::Event::KeyReleased) {
            if (event.key.code == sf::Keyboard::Space || event.key.code == sf::Keyboard::Up) {
                if (!is_falling) {
                    dino.set_velocity(0);
                }
            }
            else if (event.key.code == sf::Keyboard::Down) {
                if (!is_jumping && !is_falling && !is_pause) {
                    dino.set_crouch(false);
                }
            }
        }
    }
}

void Game::update(double dt)
{
    if (is_lose)
        return;

    if (!is_pause) {
        if (score_timer.getElapsedTime().asSeconds() > 0.05f) {
            ++score;
            if (score % 200 == 0) {
                score_sound.play();
            }
            score_timer.restart();
        }
    }

    if (!is_jumping && !is_falling && !is_pause) {
        dino.set_crouch(sf::Keyboard::isKeyPressed(sf::Keyboard::Down));
    }

    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && !is_jumping && !is_falling && !is_pause) {
        dino.set_crouch(false);
        dino.set_velocity(get_init_jump_velocity(c_max_height));
        is_jumping = true;
        jump_sound.play();
    }

    dino.set_velocity(dino.get_velocity() - c_gravity * dt);
    dino.move(dt);
    if (is_jumping && dino.get_velocity() <= 0) {
        is_falling = true;
        is_jumping = false;
    }

    // Obstacle collision
    if (dino.get_hitbox_shape().intersects(obstacles.front().get_hitbox_shape())) {
        is_lose = true;
        // pause_text.setString("Game Over");
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
            pre_score        = score;
            obstacle_speed   = get_obstacle_speed(score, obstacle_speed);
            obstacle_spacing = get_obstacle_spacing(score, obstacle_spacing);
        }
    }

    for (int i = 0; i < obstacles.size(); ++i) {
        obstacles[i].set_velocity(obstacle_speed);
        obstacles[i].move(dt);
    }
}

void Game::render()
{
    if (is_lose)
        pause_text.setString("Game Over");

    if (score % 1000 == 0 && score != 0) {
        static int pre_score = 0;
        if (pre_score != score) {
            pre_score = score;

            left_pad.setFillColor(get_opposite_theme(background_color));
            right_pad.setFillColor(get_opposite_theme(background_color));
            ground_pad.setFillColor(get_opposite_theme(ground_pad.getFillColor()));
            score_text.setFillColor(get_opposite_theme(score_text.getFillColor()));
            pause_text.setFillColor(get_opposite_theme(pause_text.getFillColor()));

            background_color = get_opposite_theme(background_color);
        }
    }

    if (is_pause || is_lose) {
        pause_text.setFillColor(get_opposite_theme(background_color));
    }
    else {
        pause_text.setFillColor(background_color);
    }

    score_text.setString(std::to_string(score));

    window.clear(background_color);
    window.draw(dino);
    for (const auto& obstacle : obstacles) {
        window.draw(obstacle);
    }
    window.draw(left_pad);
    window.draw(right_pad);
    window.draw(ground_pad);
    window.draw(pause_text);
    window.draw(score_text);

    window.display();
}

void Game::run()
{
    /*while (window.isOpen()) {
        sf::Time time = sf::Time::Zero;
        process_events();
        if (!is_pause && !is_lose) {
            time = clock.restart();
        }
        else {
            clock.restart();
        }
        update(time.asSeconds());
        output();
    }*/

    // Fixed time step
    sf::Time time_since_last_update = sf::Time::Zero;
    while (window.isOpen()) {
        process_events();

        if (!is_pause && !is_lose) {
            time_since_last_update += clock.restart();
        }
        else {
            clock.restart();
        }
        
        while (time_since_last_update >= time_per_frame) {
            time_since_last_update -= time_per_frame;
            process_events();
            update(time_per_frame.asSeconds());
        }
        render();
    }
}
