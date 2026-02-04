#pragma once

#include <SFML/Graphics.hpp>
#include <string>

namespace game::player {

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
    sf::Vector2i m_frameSize{16,16};
    unsigned int m_framesPerRow = 4;
    unsigned int m_currentFrame = 0;
    sf::Time m_frameTime = sf::seconds(0.04f);
    sf::Time m_frameAcc = sf::Time::Zero;

    State m_state = State::Idle;
    sf::Vector2i m_directionCell{0,0}; // row, col

    // sword attack
    sf::ConvexShape m_sword;
    bool m_isAttacking = false;
    sf::Time m_attackDuration = sf::seconds(0.3f);
    sf::Time m_attackTimer = sf::Time::Zero;
    sf::Vector2f m_lastDirection{0.f, 1.f}; // default facing down
    float m_swordDistance = 32.f; // covers same distance as character size
};

} // namespace game::player
