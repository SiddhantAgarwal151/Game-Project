#pragma once

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

namespace game::entities {

class Entity {
public:
    virtual ~Entity() = default;
    virtual void update(const sf::Time& dt) = 0;
    virtual void draw(sf::RenderTarget& target) const = 0;
};

} // namespace game::entities
