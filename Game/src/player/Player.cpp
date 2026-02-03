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
    sf::Vector2f dir{0.f, 0.f};
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) dir.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) dir.x += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) dir.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) dir.y += 1.f;

    if (dir.x != 0.f || dir.y != 0.f) {
        m_state = State::Walk;
        if (std::abs(dir.x) >= std::abs(dir.y)) {
            m_directionCell.x = (dir.x < 0) ? 1 : 2; // left row=1, right row=2
        } else {
            m_directionCell.x = (dir.y < 0) ? 3 : 0; // up row=3, down row=0
        }
    } else {
        m_state = State::Idle;
    }
}

void Player::update(const sf::Time& dt)
{
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

    if (m_state == State::Walk) {
        m_frameAcc += dt;
        if (m_frameAcc >= m_frameTime) {
            m_frameAcc -= m_frameTime;
            m_currentFrame = (m_currentFrame + 1) % m_framesPerRow;
        }
    } else {
        m_currentFrame = 0;
    }

    int left = static_cast<int>(m_currentFrame) * m_frameSize.x;
    int top = static_cast<int>(m_directionCell.x) * m_frameSize.y;
    m_sprite.setTextureRect(sf::IntRect({left, top}, {m_frameSize.x, m_frameSize.y}));
    m_sprite.setPosition(m_position);
}

void Player::draw(sf::RenderTarget& target) const
{
    target.draw(m_sprite);
}

} // namespace game::player
