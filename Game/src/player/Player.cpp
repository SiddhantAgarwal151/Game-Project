#include "Player.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <iostream>
#include <cmath>

namespace game::player {

Player::Player()
    : m_sprite(m_texture)
{}

bool Player::load(const std::string& texturePath, const sf::Vector2i& frameSize, unsigned int framesPerRow)
{
    m_frameSize = frameSize;
    m_framesPerRow = framesPerRow;

    if (!m_texture.loadFromFile(texturePath)) {
        std::cerr << "Failed to load image: " << texturePath << " — generating placeholder spritesheet.\n";

        // Create a placeholder image: 4 rows (directions) x framesPerRow columns
        unsigned int rows = 4;
        unsigned int w = m_framesPerRow * static_cast<unsigned int>(m_frameSize.x);
        unsigned int h = rows * static_cast<unsigned int>(m_frameSize.y);

        sf::Image image({w, h}, sf::Color::Transparent);

        // Fill each frame with a distinct color so animation and direction are visible
        for (unsigned int row = 0; row < rows; ++row) {
            for (unsigned int col = 0; col < m_framesPerRow; ++col) {
                // color varies by row and column
                std::uint8_t r = static_cast<std::uint8_t>((50 + row * 40) % 256);
                std::uint8_t g = static_cast<std::uint8_t>((100 + col * 60) % 256);
                std::uint8_t b = static_cast<std::uint8_t>((150 + row * 20 + col * 30) % 256);
                sf::Color fill(r, g, b, 255);

                unsigned int x0 = col * static_cast<unsigned int>(m_frameSize.x);
                unsigned int y0 = row * static_cast<unsigned int>(m_frameSize.y);
                for (unsigned int y = 0; y < static_cast<unsigned int>(m_frameSize.y); ++y) {
                    for (unsigned int x = 0; x < static_cast<unsigned int>(m_frameSize.x); ++x) {
                        image.setPixel({x0 + x, y0 + y}, fill);
                    }
                }

                // draw a small darker square to indicate frame index
                for (unsigned int y = 4; y < 10 && y < static_cast<unsigned int>(m_frameSize.y)-4; ++y) {
                    for (unsigned int x = 4; x < 10 && x < static_cast<unsigned int>(m_frameSize.x)-4; ++x) {
                        image.setPixel({x0 + x, y0 + y}, sf::Color::Black);
                    }
                }
            }
        }

        if (!m_texture.loadFromImage(image)) {
            std::cerr << "Failed to create placeholder texture\n";
            return false;
        }
    }

    m_sprite.setTexture(m_texture);
    m_sprite.setTextureRect(sf::IntRect({0,0}, {m_frameSize.x, m_frameSize.y}));
    m_sprite.setOrigin(sf::Vector2f(0.f, 0.f));
    
    // Debug output
    sf::Vector2u texSize = m_texture.getSize();
    std::cout << "Texture loaded: " << texturePath << " Size: " << texSize.x << "x" << texSize.y 
              << " Frame size: " << m_frameSize.x << "x" << m_frameSize.y 
              << " Frames per row: " << m_framesPerRow << "\n";
    
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
    sf::Vector2f dir{0.f, 0.f};
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) dir.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) dir.x += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) dir.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) dir.y += 1.f;

    // Store direction for sword attack
    if (dir.x != 0.f || dir.y != 0.f) {
        m_lastDirection = dir;
    }

    // Attack on Space or LCtrl
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
        if (!m_isAttacking) {
            m_isAttacking = true;
            m_attackTimer = sf::Time::Zero;
            m_state = State::Attack;
        }
    }
}

void Player::update(const sf::Time& dt)
{
    sf::Vector2f move{0.f, 0.f};
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) move.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) move.x += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) move.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) move.y += 1.f;

    if (!m_isAttacking) {
        if (move.x != 0.f || move.y != 0.f) {
            m_state = State::Walk;
            // Update direction based on movement
            // Row mapping: 0=down, 1=up, 2=right, 3=left
            if (std::abs(move.x) >= std::abs(move.y)) {
                m_directionCell.x = (move.x < 0) ? 3 : 2; // left row=3, right row=2
            } else {
                m_directionCell.x = (move.y < 0) ? 1 : 0; // up row=1, down row=0
            }
            
            const float len = std::sqrt(move.x*move.x + move.y*move.y);
            if (len > 0.f) move /= len;
            m_position += move * m_speed * dt.asSeconds();
            
            // Update animation frame
            m_frameAcc += dt;
            if (m_frameAcc >= m_frameTime) {
                m_frameAcc -= m_frameTime;
                m_currentFrame = (m_currentFrame + 1) % m_framesPerRow;
            }
        } else {
            m_state = State::Idle;
            m_currentFrame = 0;
            m_frameAcc = sf::Time::Zero;
        }
    } else {
        // Update attack timer
        m_attackTimer += dt;
        if (m_attackTimer >= m_attackDuration) {
            m_isAttacking = false;
            m_attackTimer = sf::Time::Zero;
            m_state = State::Idle;
        }
        m_currentFrame = 0;
    }

    // Ensure direction cell is valid (default to down if somehow invalid)
    if (m_directionCell.x < 0 || m_directionCell.x > 3) {
        m_directionCell.x = 0;
    }

    // Clamp frame to valid range
    if (m_currentFrame >= m_framesPerRow) {
        m_currentFrame = 0;
    }

    int left = m_currentFrame * m_frameSize.x;
    int top = m_directionCell.x * m_frameSize.y;
    
    m_sprite.setTextureRect(sf::IntRect({left, top}, {m_frameSize.x, m_frameSize.y}));
    m_sprite.setPosition(m_position);

    // Debug output (print once per second)
    static sf::Clock debugClock;
    if (debugClock.getElapsedTime().asSeconds() > 1.f) {
        std::cout << "Frame: " << m_currentFrame << " Dir: " << m_directionCell.x 
                  << " Left: " << left << " Top: " << top 
                  << " FrameSize: " << m_frameSize.x << "x" << m_frameSize.y << "\n";
        debugClock.restart();
    }

    // Update sword position and rotation (don't modify m_lastDirection)
    sf::Vector2f swordDir = m_lastDirection;
    float len = std::sqrt(swordDir.x * swordDir.x + swordDir.y * swordDir.y);
    if (len > 0.f) {
        swordDir /= len;
    }

    float angle = std::atan2(swordDir.y, swordDir.x) * 180.f / 3.14159f;

    // Create sword shape (triangle)
    m_sword.setPointCount(3);
    m_sword.setPoint(0, sf::Vector2f(m_swordDistance, 0.f));      // tip
    m_sword.setPoint(1, sf::Vector2f(0.f, -8.f));                 // left
    m_sword.setPoint(2, sf::Vector2f(0.f, 8.f));                  // right
    m_sword.setFillColor(sf::Color(200, 200, 200, m_isAttacking ? 255 : 0));
    m_sword.setPosition(m_position);
    m_sword.setRotation(sf::degrees(angle));
}

void Player::draw(sf::RenderTarget& target) const
{
    target.draw(m_sprite);
    if (m_isAttacking) {
        target.draw(m_sword);
    }
}

} // namespace game::player
