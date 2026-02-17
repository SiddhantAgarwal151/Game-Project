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

bool Player::checkCollision(const sf::CircleShape& circle) const
{
    // Get player bounds
    sf::FloatRect playerRect = getBounds();
    
    // Get circle position and radius
    sf::Vector2f circlePos = circle.getPosition();
    float radius = circle.getRadius();
    
    // Adjust circle position (since your projectile sets position as top-left corner)
    sf::Vector2f circleCenter = circlePos + sf::Vector2f(radius, radius);
    
    // SFML 3 uses position and size instead of left/top/width/height
    float rectLeft = playerRect.position.x;
    float rectTop = playerRect.position.y;
    float rectRight = playerRect.position.x + playerRect.size.x;
    float rectBottom = playerRect.position.y + playerRect.size.y;
    
    // Find the closest point on the rectangle to the circle center
    float closestX = std::max(rectLeft, std::min(circleCenter.x, rectRight));
    float closestY = std::max(rectTop, std::min(circleCenter.y, rectBottom));
    
    // Calculate distance between circle center and closest point
    float distanceX = circleCenter.x - closestX;
    float distanceY = circleCenter.y - closestY;
    float distanceSquared = distanceX * distanceX + distanceY * distanceY;
    
    // Check if distance is less than radius
    bool collision = distanceSquared < (radius * radius);
    
    // Debug output
    if (collision) {
        std::cout << "COLLISION DETECTED! Player at (" << m_position.x << ", " << m_position.y 
                  << ") Projectile at (" << circleCenter.x << ", " << circleCenter.y << ")\n";
    }
    
    return collision;
}

void Player::takeDamage(float amount)
{
    // Comment out or remove these lines:
    // if (m_isInvincible) {
    //     std::cout << "Player is invincible, damage blocked!\n";
    //     return;
    // }
    
    m_health = std::max(0.f, m_health - amount);
    std::cout << "Player took " << amount << " damage! Health: " << m_health << "/" << m_maxHealth << "\n";
    
    // Keep the flashing effect for visual feedback
    m_isInvincible = true;
    m_invincibilityTimer = sf::Time::Zero;
    
    if (m_health <= 0.f) {
        std::cout << "Player died!\n";
    }
}

void Player::heal(float amount)
{
    m_health = std::min(m_maxHealth, m_health + amount);
}

void Player::handleInput()
{
    sf::Vector2f dir{0.f, 0.f};
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) dir.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) dir.x += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) dir.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) dir.y += 1.f;

    if (dir.x != 0.f || dir.y != 0.f) {
        m_lastDirection = dir;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
        if (!m_isAttacking) {
            m_isAttacking = true;
            m_attackTimer = sf::Time::Zero;
            m_state = State::Attack;
        }
    }
    
    // Debug: damage and heal with keys
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::H)) {
        static sf::Clock healClock;
        if (healClock.getElapsedTime().asSeconds() > 0.5f) {
            heal(0.5f);
            healClock.restart();
        }
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::J)) {
        static sf::Clock damageClock;
        if (damageClock.getElapsedTime().asSeconds() > 0.5f) {
            takeDamage(0.5f);
            damageClock.restart();
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
            if (std::abs(move.x) >= std::abs(move.y)) {
                m_directionCell.x = (move.x < 0) ? 3 : 2;
            } else {
                m_directionCell.x = (move.y < 0) ? 1 : 0;
            }
            
            const float len = std::sqrt(move.x*move.x + move.y*move.y);
            if (len > 0.f) move /= len;
            m_position += move * m_speed * dt.asSeconds();
            
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
        m_attackTimer += dt;
        if (m_attackTimer >= m_attackDuration) {
            m_isAttacking = false;
            m_attackTimer = sf::Time::Zero;
            m_state = State::Idle;
        }
        m_currentFrame = 0;
    }

    if (m_directionCell.x < 0 || m_directionCell.x > 3) {
        m_directionCell.x = 0;
    }

    if (m_currentFrame >= m_framesPerRow) {
        m_currentFrame = 0;
    }

    int left = m_currentFrame * m_frameSize.x;
    int top = m_directionCell.x * m_frameSize.y;
    
    m_sprite.setTextureRect(sf::IntRect({left, top}, {m_frameSize.x, m_frameSize.y}));
    m_sprite.setPosition(m_position);

    static sf::Clock debugClock;
    if (debugClock.getElapsedTime().asSeconds() > 1.f) {
        std::cout << "Frame: " << m_currentFrame << " Dir: " << m_directionCell.x 
                  << " Left: " << left << " Top: " << top 
                  << " FrameSize: " << m_frameSize.x << "x" << m_frameSize.y << "\n";
        debugClock.restart();
    }

    sf::Vector2f swordDir = m_lastDirection;
    float len = std::sqrt(swordDir.x * swordDir.x + swordDir.y * swordDir.y);
    if (len > 0.f) {
        swordDir /= len;
    }

    float angle = std::atan2(swordDir.y, swordDir.x) * 180.f / 3.14159f;

    m_sword.setPointCount(3);
    m_sword.setPoint(0, sf::Vector2f(m_swordDistance, 0.f));
    m_sword.setPoint(1, sf::Vector2f(0.f, -8.f));
    m_sword.setPoint(2, sf::Vector2f(0.f, 8.f));
    m_sword.setFillColor(sf::Color(200, 200, 200, m_isAttacking ? 255 : 0));
    m_sword.setPosition(m_position);
    m_sword.setRotation(sf::degrees(angle));
}

sf::ConvexShape Player::createHeart(const sf::Vector2f& position, float size) const
{
    sf::ConvexShape heart;
    heart.setPointCount(12);
    
    // Create a heart shape using bezier-like approximation
    float s = size;
    heart.setPoint(0, sf::Vector2f(0.f, s * 0.3f));          // bottom point
    heart.setPoint(1, sf::Vector2f(-s * 0.25f, s * 0.1f));
    heart.setPoint(2, sf::Vector2f(-s * 0.4f, -s * 0.1f));
    heart.setPoint(3, sf::Vector2f(-s * 0.45f, -s * 0.3f));  // left top curve
    heart.setPoint(4, sf::Vector2f(-s * 0.4f, -s * 0.45f));
    heart.setPoint(5, sf::Vector2f(-s * 0.2f, -s * 0.5f));   // left bump top
    heart.setPoint(6, sf::Vector2f(0.f, -s * 0.4f));         // center dip
    heart.setPoint(7, sf::Vector2f(s * 0.2f, -s * 0.5f));    // right bump top
    heart.setPoint(8, sf::Vector2f(s * 0.4f, -s * 0.45f));
    heart.setPoint(9, sf::Vector2f(s * 0.45f, -s * 0.3f));   // right top curve
    heart.setPoint(10, sf::Vector2f(s * 0.4f, -s * 0.1f));
    heart.setPoint(11, sf::Vector2f(s * 0.25f, s * 0.1f));
    
    heart.setPosition(position);
    return heart;
}

void Player::drawHearts(sf::RenderTarget& target) const
{
    const float heartSize = 16.f;
    const float heartSpacing = 20.f;
    const sf::Vector2f startPos(20.f, 20.f);
    
    int totalHearts = static_cast<int>(std::ceil(m_maxHealth / 2.f));
    
    for (int i = 0; i < totalHearts; ++i) {
        float heartValue = m_health - (i * 2.f);
        sf::ConvexShape heart = createHeart(
            sf::Vector2f(startPos.x + i * heartSpacing, startPos.y),
            heartSize
        );
        
        // Full heart
        if (heartValue >= 2.f) {
            heart.setFillColor(sf::Color::Red);
            heart.setOutlineColor(sf::Color(139, 0, 0)); // Dark red
            heart.setOutlineThickness(1.f);
        }
        // Half heart
        else if (heartValue >= 1.f) {
            heart.setFillColor(sf::Color(255, 100, 100)); // Light red
            heart.setOutlineColor(sf::Color(139, 0, 0));
            heart.setOutlineThickness(1.f);
        }
        // Empty heart
        else {
            heart.setFillColor(sf::Color(50, 50, 50)); // Dark gray
            heart.setOutlineColor(sf::Color(100, 100, 100));
            heart.setOutlineThickness(1.f);
        }
        
        target.draw(heart);
    }
}

void Player::draw(sf::RenderTarget& target) const
{
    target.draw(m_sprite);
    if (m_isAttacking) {
        target.draw(m_sword);
    }
    drawHearts(target);
}

sf::FloatRect Player::getBounds() const
{
    // Return a bounding box centered on the player
    float halfWidth = m_frameSize.x / 2.f;
    float halfHeight = m_frameSize.y / 2.f;
    
    // SFML 3: Use position and size instead of left/top/width/height
    return sf::FloatRect(
        sf::Vector2f(m_position.x - halfWidth, m_position.y - halfHeight),  // position
        sf::Vector2f(static_cast<float>(m_frameSize.x), static_cast<float>(m_frameSize.y))  // size
    );
}

// Add this method to Player.cpp:
sf::FloatRect Player::getSwordBounds() const
{
    if (!m_isAttacking) {
        return sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(0.f, 0.f));
    }
    
    // Calculate sword hitbox based on direction
    sf::Vector2f swordDir = m_lastDirection;
    float len = std::sqrt(swordDir.x * swordDir.x + swordDir.y * swordDir.y);
    if (len > 0.f) {
        swordDir /= len;
    }
    
    // Sword extends from player position in the direction they're facing
    sf::Vector2f swordPos = m_position + swordDir * (m_swordDistance / 2.f);
    float swordWidth = 16.f;
    float swordLength = m_swordDistance;
    
    return sf::FloatRect(
        sf::Vector2f(swordPos.x - swordWidth / 2.f, swordPos.y - swordLength / 2.f),
        sf::Vector2f(swordWidth, swordLength)
    );
}

} // namespace game::player