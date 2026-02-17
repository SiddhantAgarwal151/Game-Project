#include "Octorok.hpp"
#include <cmath>
#include <random>
#include <iostream>

namespace game::enemies {

Octorok::Octorok(const sf::Vector2f& position)
    : Enemy(position, 3.f, 80.f)  // 3 health, 80 speed
{
    m_shape.setFillColor(sf::Color::Red);
    m_shape.setPosition(m_position - sf::Vector2f(16.f, 16.f));
}

void Octorok::update(const sf::Time& dt)
{
    if (!isAlive()) return;
    
    // Call base class update for flash effect
    Enemy::update(dt);
    
    // Move based on direction
    m_position += m_moveDirection * m_speed * dt.asSeconds();
    m_shape.setPosition(m_position - sf::Vector2f(16.f, 16.f));
    
    // Update shoot timer
    m_shootTimer += dt;
    if (m_shootTimer.asSeconds() >= m_shootCooldown) {
        shootProjectile();
        m_shootTimer = sf::Time::Zero;
    }
    
    // Update projectiles and remove out of bounds ones
    for (auto it = m_projectiles.begin(); it != m_projectiles.end(); ) {
        (*it)->update(dt);
        if (!(*it)->isAlive() || (*it)->isOutOfBounds({1000.f, 800.f})) {
            it = m_projectiles.erase(it);
        } else {
            ++it;
        }
    }
}

void Octorok::draw(sf::RenderTarget& target) const
{
    // Draw enemy body with flash effect
    Enemy::draw(target);
    
    // Draw projectiles
    for (const auto& proj : m_projectiles) {
        proj->draw(target);
    }
}

void Octorok::updateAI(const sf::Vector2f& playerPos)
{
    // Calculate distance to player
    sf::Vector2f dir = playerPos - m_position;
    float distance = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    
    if (distance < m_detectionRange && distance > 1.f) {
        // Normalize direction
        dir /= distance;
        m_moveDirection = dir;
    } else {
        // Wander behavior - change direction occasionally
        m_moveTimer += sf::milliseconds(16);
        if (m_moveTimer.asSeconds() > 2.f) {
            // Random direction
            static std::mt19937 gen(std::random_device{}());
            static std::uniform_real_distribution<float> angleDist(0.f, 6.28318f);
            float angle = angleDist(gen);
            m_moveDirection = {std::cos(angle), std::sin(angle)};
            m_moveTimer = sf::Time::Zero;
        }
    }
}

void Octorok::shootProjectile()
{
    // Shoot projectile in the direction the octorok is moving
    sf::Vector2f shootDir = m_moveDirection;
    float projectileSpeed = 150.f;
    auto projectile = std::make_shared<game::projectiles::Projectile>(
        m_position, 
        shootDir * projectileSpeed
    );
    m_projectiles.push_back(projectile);
}

} // namespace game::enemies