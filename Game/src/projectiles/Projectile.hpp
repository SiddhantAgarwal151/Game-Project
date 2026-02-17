#pragma once
#include <SFML/Graphics.hpp>
#include "../entities/Entity.hpp"

namespace game::projectiles {
class Projectile {
public:
    Projectile(const sf::Vector2f& position, const sf::Vector2f& velocity, float radius = 4.f);
    
    void update(const sf::Time& dt);
    void draw(sf::RenderTarget& target) const;
    
    sf::Vector2f getPosition() const { return m_position; }
    sf::CircleShape getShape() const { return m_shape; }
    bool isAlive() const { return m_alive; }
    void markForDeletion() { m_alive = false; }
    
    bool isOutOfBounds(const sf::Vector2f& screenSize) const;
    
private:
    sf::Vector2f m_position;
    sf::Vector2f m_velocity; // px/sec
    sf::CircleShape m_shape;
    bool m_alive = true;
};
} // namespace game::projectiles