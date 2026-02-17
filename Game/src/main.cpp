#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include <memory>
#include <string>
#include <cstdlib>
#include <vector>
#include "player/Player.hpp"
#include "entities/enemies/Octorok.hpp"  // Changed from "enemies/" to "entities/enemies/"

int main()
{
    const char* displayEnv = std::getenv("DISPLAY");
    
    sf::Font font;
    std::unique_ptr<sf::Text> fpsText;
    std::string loadedFontPath;
    std::vector<std::string> fontCandidates = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "../assets/fonts/arial.ttf",
        "assets/fonts/arial.ttf",
        "arial.ttf"
    };
    
    bool fontLoaded = false;
    for (auto &p : fontCandidates) {
        if (font.openFromFile(p)) {
            fpsText = std::make_unique<sf::Text>(font, "", 16u);
            fpsText->setFillColor(sf::Color::White);
            fpsText->setPosition({8.f, 8.f});
            loadedFontPath = p;
            fontLoaded = true;
            std::cout << "Font loaded from " << p << "\n";
            break;
        }
    }
    
    game::player::Player player;
    player.load("assets/sprites/link_64x64_spritesheet.png", {16, 16}, 4);
    
    if (!displayEnv) {
        std::cout << "No DISPLAY detected\n";
        return 0;
    }
    
    sf::RenderWindow window(sf::VideoMode({800u, 600u}), "Game - Combat System");
    window.setVerticalSyncEnabled(true);
    
    sf::Clock clock;
    player.setPosition({400.f, 300.f});
    
    // CREATE ENEMIES
    std::vector<std::shared_ptr<game::enemies::Octorok>> enemies;
    enemies.push_back(std::make_shared<game::enemies::Octorok>(sf::Vector2f{200.f, 150.f}));
    enemies.push_back(std::make_shared<game::enemies::Octorok>(sf::Vector2f{600.f, 150.f}));
    enemies.push_back(std::make_shared<game::enemies::Octorok>(sf::Vector2f{200.f, 450.f}));
    enemies.push_back(std::make_shared<game::enemies::Octorok>(sf::Vector2f{600.f, 450.f}));
    
    // Game Over screen
    sf::RectangleShape gameOverOverlay({800.f, 600.f});
    gameOverOverlay.setFillColor(sf::Color(0, 0, 0, 200));
    
    std::unique_ptr<sf::Text> gameOverText;
    std::unique_ptr<sf::Text> tryAgainText;
    bool gameOver = false;
    
    if (fontLoaded) {
        gameOverText = std::make_unique<sf::Text>(font, "GAME OVER", 72u);
        gameOverText->setFillColor(sf::Color::Red);
        gameOverText->setOutlineColor(sf::Color::White);
        gameOverText->setOutlineThickness(3.f);
        
        sf::FloatRect bounds = gameOverText->getLocalBounds();
        gameOverText->setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
        gameOverText->setPosition({400.f, 250.f});
        
        tryAgainText = std::make_unique<sf::Text>(font, "Press SPACE to Try Again", 30u);
        tryAgainText->setFillColor(sf::Color::Yellow);
        tryAgainText->setOutlineColor(sf::Color::Black);
        tryAgainText->setOutlineThickness(2.f);
        
        bounds = tryAgainText->getLocalBounds();
        tryAgainText->setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
        tryAgainText->setPosition({400.f, 350.f});
    }
    
    while (window.isOpen())
    {
        sf::Time dt = clock.restart();
        
        while (const auto ev = window.pollEvent())
        {
            if (ev->is<sf::Event::Closed>())
                window.close();
            if (auto key = ev->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                    window.close();
                    
                if (gameOver && key->code == sf::Keyboard::Key::Space) {
                    gameOver = false;
                    player = game::player::Player();
                    player.load("assets/sprites/link_64x64_spritesheet.png", {16, 16}, 4);
                    player.setPosition({400.f, 300.f});
                    
                    enemies.clear();
                    enemies.push_back(std::make_shared<game::enemies::Octorok>(sf::Vector2f{200.f, 150.f}));
                    enemies.push_back(std::make_shared<game::enemies::Octorok>(sf::Vector2f{600.f, 150.f}));
                    enemies.push_back(std::make_shared<game::enemies::Octorok>(sf::Vector2f{200.f, 450.f}));
                    enemies.push_back(std::make_shared<game::enemies::Octorok>(sf::Vector2f{600.f, 450.f}));
                }
            }
        }
        
        if (!player.isAlive() && !gameOver) {
            gameOver = true;
            std::cout << "=== GAME OVER ===\n";
        }
        
        if (!gameOver) {
            player.handleInput();
            player.update(dt);
            
            // Get sword bounds for collision
            sf::FloatRect swordBounds = player.getSwordBounds();
            
            for (auto& enemy : enemies) {
                if (enemy->isAlive()) {
                    enemy->updateAI(player.getPosition());
                    enemy->update(dt);
                    
                    // Check sword collision with enemy
                    if (player.isAttacking() && 
                        swordBounds.findIntersection(enemy->getBounds()).has_value()) {
                        enemy->takeDamage(1.f);  // 1 damage per hit
                    }
                    
                    // Check projectile collisions with player
                    auto& projectiles = enemy->getProjectiles();
                    for (auto& projectile : projectiles) {
                        if (projectile->isAlive() && 
                            player.checkCollision(projectile->getShape())) {
                            projectile->markForDeletion();
                            player.takeDamage(0.5f);
                        }
                    }
                }
            }
            
            // Remove dead enemies
            enemies.erase(
                std::remove_if(enemies.begin(), enemies.end(),
                    [](const auto& e) { return !e->isAlive(); }),
                enemies.end()
            );
        }
        
        if (fpsText) {
            float fps = 1.f / std::max(1e-6f, dt.asSeconds());
            fpsText->setString("FPS: " + std::to_string(static_cast<int>(fps + 0.5f)));
        }
        
        window.clear(sf::Color::Black);
        
        if (gameOver) {
            window.draw(gameOverOverlay);
            if (gameOverText && tryAgainText) {
                window.draw(*gameOverText);
                window.draw(*tryAgainText);
            }
        } else {
            for (const auto& enemy : enemies) {
                if (enemy->isAlive()) {
                    enemy->draw(window);
                }
            }
            
            player.draw(window);
            
            if (fpsText) window.draw(*fpsText);
        }
        
        window.display();
    }
    
    return 0;
}