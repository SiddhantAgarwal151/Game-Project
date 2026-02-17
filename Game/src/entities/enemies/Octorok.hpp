#pragma once
#include "../Enemy.hpp"  // Go up one directory to entities/
#include "../../projectiles/Projectile.hpp"  // Go up two directories
#include <vector>
#include <memory>

namespace game::enemies {

class Octorok : public Enemy {
public:
    Octorok(const sf::Vector2f& position);
    
    void update(const sf::Time& dt) override;
    void draw(sf::RenderTarget& target) const override;
    
    void updateAI(const sf::Vector2f& playerPos) override;
    
    // Projectile management
    std::vector<std::shared_ptr<game::projectiles::Projectile>>& getProjectiles() 
    { 
        return m_projectiles; 
    }
    
    const std::vector<std::shared_ptr<game::projectiles::Projectile>>& getProjectiles() const 
    { 
        return m_projectiles; 
    }
    
private:
    void shootProjectile();
    
    float m_detectionRange = 300.f;
    float m_shootCooldown = 1.5f;
    sf::Time m_shootTimer = sf::Time::Zero;
    sf::Time m_moveTimer = sf::Time::Zero;
    
    std::vector<std::shared_ptr<game::projectiles::Projectile>> m_projectiles;
};

} // namespace game::enemies