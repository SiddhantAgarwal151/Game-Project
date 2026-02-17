#include "Projectile.hpp"

namespace game::projectiles {

Projectile::Projectile(const sf::Vector2f& position, const sf::Vector2f& velocity, float radius)
    : m_position(position), m_velocity(velocity), m_shape(radius)
{
    m_shape.setFillColor(sf::Color::Yellow);
    m_shape.setPosition(m_position - sf::Vector2f(radius, radius));
}

void Projectile::update(const sf::Time& dt)
{
    m_position += m_velocity * dt.asSeconds();
    m_shape.setPosition(m_position - sf::Vector2f(m_shape.getRadius(), m_shape.getRadius()));
}

void Projectile::draw(sf::RenderTarget& target) const
{
    if (m_alive) {
        target.draw(m_shape);
    }
}

bool Projectile::isOutOfBounds(const sf::Vector2f& screenSize) const
{
    return m_position.x < -50.f || m_position.x > screenSize.x + 50.f ||
           m_position.y < -50.f || m_position.y > screenSize.y + 50.f;
}

} // namespace game::projectiles