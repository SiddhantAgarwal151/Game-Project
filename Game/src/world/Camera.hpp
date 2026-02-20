#pragma once
#include <SFML/Graphics.hpp>

namespace game::world {

class Camera {
public:
    Camera(const sf::Vector2f& viewSize, const sf::FloatRect& worldBounds);
    
    void update(const sf::Vector2f& targetPosition);
    void apply(sf::RenderWindow& window);
    
    sf::View& getView() { return m_view; }
    const sf::View& getView() const { return m_view; }
    
    sf::Vector2f getPosition() const { return m_view.getCenter(); }
    sf::FloatRect getViewBounds() const;
    
    void setWorldBounds(const sf::FloatRect& bounds) { m_worldBounds = bounds; }
    
private:
    sf::View m_view;
    sf::FloatRect m_worldBounds;
    float m_smoothing = 0.1f; // Camera smoothing factor (0 = instant, 1 = no movement)
};

} // namespace game::world