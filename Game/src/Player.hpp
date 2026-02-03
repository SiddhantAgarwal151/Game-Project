#pragma once

#include <SFML/Graphics.hpp>

class Player {
public:
    enum class State { Idle, Walk, Attack, Dash };

    Player();
    // load sprite sheet; expected layout: rows = directions, cols = frames
    bool load(const std::string& texturePath, const sf::Vector2i& frameSize = {32,32}, unsigned int framesPerRow = 3);

    void update(const sf::Time& dt);
    void handleInput();
    void draw(sf::RenderTarget& target) const;

    void setPosition(const sf::Vector2f& pos);
    sf::Vector2f getPosition() const;

private:
    sf::Texture m_texture;
    sf::Sprite m_sprite;

    sf::Vector2f m_position{0.f,0.f};
    float m_speed = 140.f; // px / sec

    // animation
    sf::Vector2i m_frameSize{32,32};
    unsigned int m_framesPerRow = 3;
    unsigned int m_currentFrame = 0;
    sf::Time m_frameTime = sf::seconds(0.12f);
    sf::Time m_frameAcc = sf::Time::Zero;

    State m_state = State::Idle;
    sf::Vector2i m_directionCell{0,0}; // row, col
};
