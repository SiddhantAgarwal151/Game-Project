#pragma once
#include "Entity.hpp"
#include <SFML/Graphics.hpp>

namespace game::enemies {

class Enemy : public game::entities::Entity {
public:
    Enemy(const sf::Vector2f& position, float health, float speed);
    virtual ~Enemy() = default;
    
    // Entity interface
    void update(const sf::Time& dt) override;
    void draw(sf::RenderTarget& target) const override;
    
    sf::Vector2f getPosition() const override { return m_position; }
    void setPosition(const sf::Vector2f& pos) override;
    
    float getHealth() const override { return m_health; }
    float getMaxHealth() const override { return m_maxHealth; }
    void takeDamage(float amount) override;
    bool isAlive() const override { return m_health > 0.f; }
    
    sf::FloatRect getBounds() const override;
    
    // Enemy-specific
    virtual void updateAI(const sf::Vector2f& playerPos) = 0;
    
protected:
    sf::Vector2f m_position;
    float m_health;
    float m_maxHealth;
    float m_speed;
    
    sf::CircleShape m_shape;
    sf::Vector2f m_moveDirection{1.f, 0.f};
    
    // Damage flash effect
    bool m_isFlashing = false;
    sf::Time m_flashTimer = sf::Time::Zero;
    sf::Time m_flashDuration = sf::seconds(0.2f);
};

} // namespace game::enemies