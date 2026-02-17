#pragma once
#include <SFML/Graphics.hpp>

namespace game::entities {

class Entity {
public:
    virtual ~Entity() = default;
    
    virtual void update(const sf::Time& dt) = 0;
    virtual void draw(sf::RenderTarget& target) const = 0;
    
    virtual sf::Vector2f getPosition() const = 0;
    virtual void setPosition(const sf::Vector2f& pos) = 0;
    
    virtual float getHealth() const = 0;
    virtual float getMaxHealth() const = 0;
    virtual void takeDamage(float amount) = 0;
    virtual bool isAlive() const = 0;
    
    virtual sf::FloatRect getBounds() const = 0;
};

} // namespace game::entities