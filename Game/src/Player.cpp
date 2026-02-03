#include "Player.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <iostream>
#include <cmath>
#include <vector>

Player::Player()
    : m_sprite(m_texture)
{}

bool Player::load(const std::string& texturePath, const sf::Vector2i& frameSize, unsigned int framesPerRow)
{
    m_frameSize = frameSize;
    m_framesPerRow = framesPerRow;

    if (!m_texture.loadFromFile(texturePath)) {
        std::cerr << "Failed to load player texture: " << texturePath << "\n";
        return false;
    }

    // Create sprite and set initial texture rect
    // sprite was constructed with reference to m_texture in ctor
    m_sprite.setTextureRect(sf::IntRect({0,0}, {m_frameSize.x, m_frameSize.y}));
    m_sprite.setOrigin(sf::Vector2f(m_frameSize.x/2.f, m_frameSize.y/2.f));
    return true;
}

void Player::setPosition(const sf::Vector2f& pos)
{
    m_position = pos;
    m_sprite.setPosition(m_position);
}

sf::Vector2f Player::getPosition() const { return m_position; }

void Player::handleInput()
{
    // Basic WASD / arrow movement.
    sf::Vector2f dir{0.f, 0.f};
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) dir.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) dir.x += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) dir.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) dir.y += 1.f;

    if (dir.x != 0.f || dir.y != 0.f) {
        m_state = State::Walk;
        // set direction row based on last cardinal direction (prefer horizontal if both)
        if (std::abs(dir.x) >= std::abs(dir.y)) {
            m_directionCell.x = (dir.x < 0) ? 1 : 2; // left row=1, right row=2
        } else {
            m_directionCell.x = (dir.y < 0) ? 3 : 0; // up row=3, down row=0
        }
        // normalize
        const float len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
        if (len > 0.f) {
            dir /= len;
        }
        // movement applied in update(dt)
    } else {
        m_state = State::Idle;
    }
}

void Player::update(const sf::Time& dt)
{
    // Re-handle input here (or could be done externally)
    sf::Vector2f move{0.f, 0.f};
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) move.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) move.x += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) move.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) move.y += 1.f;

    if (move.x != 0.f || move.y != 0.f) {
        m_state = State::Walk;
    const float len = std::sqrt(move.x*move.x + move.y*move.y);
        if (len > 0.f) move /= len;
        m_position += move * m_speed * dt.asSeconds();
    } else {
        m_state = State::Idle;
    }

    // Animation update
    if (m_state == State::Walk) {
        m_frameAcc += dt;
        if (m_frameAcc >= m_frameTime) {
            m_frameAcc -= m_frameTime;
            m_currentFrame = (m_currentFrame + 1) % m_framesPerRow;
        }
    } else {
        m_currentFrame = 0;
    }

    // Update sprite texture rect based on direction row and current frame
    int left = static_cast<int>(m_currentFrame) * m_frameSize.x;
    int top = static_cast<int>(m_directionCell.x) * m_frameSize.y;
    m_sprite.setTextureRect(sf::IntRect({left, top}, {m_frameSize.x, m_frameSize.y}));
    m_sprite.setPosition(m_position);
}

void Player::draw(sf::RenderTarget& target) const
{
    target.draw(m_sprite);
}
