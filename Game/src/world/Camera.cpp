#include "Camera.hpp"
#include <algorithm>

namespace game::world {

Camera::Camera(const sf::Vector2f& viewSize, const sf::FloatRect& worldBounds)
    : m_worldBounds(worldBounds)
{
    m_view.setSize(viewSize);
    m_view.setCenter(sf::Vector2f(viewSize.x / 2.f, viewSize.y / 2.f));
}

void Camera::update(const sf::Vector2f& targetPosition)
{
    // Smooth camera follow
    sf::Vector2f currentCenter = m_view.getCenter();
    sf::Vector2f desiredCenter = targetPosition;
    
    // Lerp towards target
    sf::Vector2f newCenter = currentCenter + (desiredCenter - currentCenter) * m_smoothing;
    
    // Clamp camera to world bounds
    sf::Vector2f viewSize = m_view.getSize();
    float halfWidth = viewSize.x / 2.f;
    float halfHeight = viewSize.y / 2.f;
    
    newCenter.x = std::max(m_worldBounds.position.x + halfWidth, 
                           std::min(newCenter.x, m_worldBounds.position.x + m_worldBounds.size.x - halfWidth));
    newCenter.y = std::max(m_worldBounds.position.y + halfHeight, 
                           std::min(newCenter.y, m_worldBounds.position.y + m_worldBounds.size.y - halfHeight));
    
    m_view.setCenter(newCenter);
}

void Camera::apply(sf::RenderWindow& window)
{
    window.setView(m_view);
}

sf::FloatRect Camera::getViewBounds() const
{
    sf::Vector2f center = m_view.getCenter();
    sf::Vector2f size = m_view.getSize();
    
    return sf::FloatRect(
        sf::Vector2f(center.x - size.x / 2.f, center.y - size.y / 2.f),
        size
    );
}

} // namespace game::world