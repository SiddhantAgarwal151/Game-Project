#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include <memory>
#include <string>
#include <cstdlib>
#include <vector>
#include "player/Player.hpp"
#include "entities/enemies/Octorok.hpp"
#include "world/World.hpp"
#include "world/Camera.hpp"
#include "audio/SoundManager.hpp"

// Helper: wire up all sound callbacks for a player instance
static void connectPlayerSounds(game::player::Player& player, game::audio::SoundManager& soundManager)
{
    player.setOnWalkStartCallback([&soundManager]() {
        soundManager.playLoopingSound(game::audio::SoundEffect::PlayerWalk, 50.f);
    });

    player.setOnWalkStopCallback([&soundManager]() {
        soundManager.stopLoopingSound(game::audio::SoundEffect::PlayerWalk);
    });

    player.setOnAttackCallback([&soundManager]() {
        soundManager.playSound(game::audio::SoundEffect::PlayerAttack, 70.f);
    });

    player.setOnHitCallback([&soundManager]() {
        soundManager.playSound(game::audio::SoundEffect::PlayerHit, 80.f);
    });

    player.setOnDeathCallback([&soundManager]() {
        soundManager.stopLoopingSound(game::audio::SoundEffect::PlayerWalk);
        soundManager.playSound(game::audio::SoundEffect::GameOver, 100.f);
        soundManager.stopMusic();
    });
}

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
    
    // CREATE SOUND MANAGER
    game::audio::SoundManager soundManager;
    
    // Load sound effects
    soundManager.loadSound(game::audio::SoundEffect::PlayerWalk,   "assets/sounds/walk.wav");
    soundManager.loadSound(game::audio::SoundEffect::PlayerAttack, "assets/sounds/sword_attack.wav");
    soundManager.loadSound(game::audio::SoundEffect::PlayerHit,    "assets/sounds/hit.wav");
    soundManager.loadSound(game::audio::SoundEffect::EnemyHit,     "assets/sounds/enemy_hit.wav");
    soundManager.loadSound(game::audio::SoundEffect::EnemyDeath,   "assets/sounds/enemy_death.wav");
    soundManager.loadSound(game::audio::SoundEffect::GameOver,     "assets/sounds/game_over.wav");
    
    // Load and start background music
    soundManager.loadMusic(game::audio::Music::MainTheme, "assets/music/background.ogg");
    soundManager.playMusic(game::audio::Music::MainTheme, true, 30.f);
    
    game::player::Player player;
    player.load("assets/sprites/link_64x64_spritesheet.png", {16, 16}, 4);
    connectPlayerSounds(player, soundManager);
    
    if (!displayEnv) {
        std::cout << "No DISPLAY detected\n";
        return 0;
    }
    
    sf::RenderWindow window(sf::VideoMode({800u, 600u}), "Game - Open World");
    window.setVerticalSyncEnabled(true);
    
    // Create world (50x50 tiles, each 32px)
    game::world::World world(50, 50, 32.f);
    world.generate();
    world.loadBackgroundTexture("assets/backgrounds/grass_bg.png");
    world.loadTileset("assets/tilesets/terrain.png", {32, 32});
    
    // Create camera
    game::world::Camera camera(sf::Vector2f(800.f, 600.f), world.getWorldBounds());
    
    sf::Clock clock;
    
    // Spawn player in center of world
    sf::Vector2f worldCenter(
        world.getWorldBounds().size.x / 2.f,
        world.getWorldBounds().size.y / 2.f
    );
    player.setPosition(worldCenter);
    
    // CREATE ENEMIES scattered around the world
    std::vector<std::shared_ptr<game::enemies::Octorok>> enemies;
    auto spawnEnemies = [&enemies]() {
        enemies.clear();
        enemies.push_back(std::make_shared<game::enemies::Octorok>(sf::Vector2f{ 400.f, 300.f}));
        enemies.push_back(std::make_shared<game::enemies::Octorok>(sf::Vector2f{ 600.f, 400.f}));
        enemies.push_back(std::make_shared<game::enemies::Octorok>(sf::Vector2f{ 800.f, 500.f}));
        enemies.push_back(std::make_shared<game::enemies::Octorok>(sf::Vector2f{ 300.f, 600.f}));
        enemies.push_back(std::make_shared<game::enemies::Octorok>(sf::Vector2f{1000.f, 400.f}));
    };
    spawnEnemies();

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
        
        // Update sound manager (cleans up finished one-shot sounds)
        soundManager.update();
        
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
                    
                    soundManager.playMusic(game::audio::Music::MainTheme, true, 30.f);
                    
                    player = game::player::Player();
                    player.load("assets/sprites/link_64x64_spritesheet.png", {16, 16}, 4);
                    player.setPosition(worldCenter);
                    connectPlayerSounds(player, soundManager);

                    spawnEnemies();
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
            
            // Check world collision before committing movement
            sf::FloatRect playerBounds = player.getBounds();
            playerBounds.position = player.getPendingPosition() - sf::Vector2f(playerBounds.size.x / 2.f, playerBounds.size.y / 2.f);
            
            if (!world.checkCollision(playerBounds)) {
                player.commitPosition();
            }
            
            camera.update(player.getPosition());
            
            sf::FloatRect swordBounds = player.getSwordBounds();
            
            for (auto& enemy : enemies) {
                if (enemy->isAlive()) {
                    enemy->updateAI(player.getPosition());
                    enemy->update(dt);
                    
                    if (player.isAttacking() && 
                        swordBounds.findIntersection(enemy->getBounds()).has_value()) {
                        enemy->takeDamage(1.f);
                        soundManager.playSound(game::audio::SoundEffect::EnemyHit, 25.f);
                        
                        if (!enemy->isAlive()) {
                            soundManager.playSound(game::audio::SoundEffect::EnemyDeath, 70.f);
                        }
                    }
                    
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
            window.setView(window.getDefaultView());
            window.draw(gameOverOverlay);
            if (gameOverText && tryAgainText) {
                window.draw(*gameOverText);
                window.draw(*tryAgainText);
            }
        } else {
            camera.apply(window);
            world.draw(window, camera.getViewBounds());
            
            for (const auto& enemy : enemies) {
                if (enemy->isAlive()) {
                    enemy->draw(window);
                }
            }
            
            player.draw(window);
            
            window.setView(window.getDefaultView());
            if (fpsText) window.draw(*fpsText);
        }
        
        window.display();
    }
    
    return 0;
}