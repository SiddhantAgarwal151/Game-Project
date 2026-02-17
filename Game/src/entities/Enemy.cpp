#include "Enemy.hpp"
#include <iostream>

namespace game::enemies {

Enemy::Enemy(const sf::Vector2f& position, float health, float speed)
    : m_position(position)
    , m_health(health)
    , m_maxHealth(health)
    , m_speed(speed)
    , m_shape(16.f)
{
    m_shape.setPosition(m_position - sf::Vector2f(16.f, 16.f));
}

void Enemy::setPosition(const sf::Vector2f& pos)
{
    m_position = pos;
    m_shape.setPosition(m_position - sf::Vector2f(16.f, 16.f));
}

void Enemy::takeDamage(float amount)
{
    m_health = std::max(0.f, m_health - amount);
    m_isFlashing = true;
    m_flashTimer = sf::Time::Zero;
    
    std::cout << "Enemy took " << amount << " damage! Health: " << m_health << "/" << m_maxHealth << "\n";
    
    if (m_health <= 0.f) {
        std::cout << "Enemy died!\n";
    }
}

sf::FloatRect Enemy::getBounds() const
{
    float radius = m_shape.getRadius();
    return sf::FloatRect(
        sf::Vector2f(m_position.x - radius, m_position.y - radius),
        sf::Vector2f(radius * 2.f, radius * 2.f)
    );
}

void Enemy::update(const sf::Time& dt)
{
    // Update flash effect
    if (m_isFlashing) {
        m_flashTimer += dt;
        if (m_flashTimer >= m_flashDuration) {
            m_isFlashing = false;
            m_flashTimer = sf::Time::Zero;
        }
    }
}

void Enemy::draw(sf::RenderTarget& target) const
{
    if (isAlive()) {
        // Flash white when taking damage
        sf::CircleShape shapeCopy = m_shape;
        if (m_isFlashing) {
            shapeCopy.setFillColor(sf::Color::White);
        }
        target.draw(shapeCopy);
    }
}

} // namespace game::enemies