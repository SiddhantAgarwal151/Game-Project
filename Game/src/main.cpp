#include <SFML/Graphics.hpp>
#include <iostream>
#include <algorithm>
#include <memory>
#include <string>
#include <cstdlib>
#include "player/Player.hpp"

int main()
{
    // If there's no DISPLAY (common in WSL without X server), run a headless font check
    const char* displayEnv = std::getenv("DISPLAY");

    // Load a font if available (not required, fall back to no text)
    sf::Font font;
    std::unique_ptr<sf::Text> fpsText;
    std::string loadedFontPath;
    std::vector<std::string> fontCandidates = {
        "../assets/fonts/arial.ttf",
        "assets/fonts/arial.ttf",
        "arial.ttf"
    };
    for (auto &p : fontCandidates) {
        if (font.openFromFile(p)) {
            fpsText = std::make_unique<sf::Text>(font, "", 16u);
            fpsText->setFillColor(sf::Color::White);
            fpsText->setPosition({8.f, 8.f});
            loadedFontPath = p;
            break;
        }
    }

    // Create player early so we can generate placeholder sprites even in headless mode
    game::player::Player player;
    bool playerLoaded = player.load("assets/sprites/link_64x64_spritesheet.png", {16, 16}, 4);
    if (!playerLoaded) {
        std::cout << "Using generated placeholder player spritesheet.\n";
    } else {
        std::cout << "Successfully loaded Link walking animation sprite.\n";
    }

    if (!displayEnv) {
        std::cout << "No DISPLAY detected; running headless check.\n";
        if (!loadedFontPath.empty())
            std::cout << "Loaded font from: " << loadedFontPath << "\n";
        else
            std::cout << "Font not found. Put arial.ttf into Game/assets/fonts and re-run.\n";
        return 0;
    }

    // Create a render window
    sf::RenderWindow window(sf::VideoMode({800u, 600u}), "Game - Starter");
    window.setVerticalSyncEnabled(true);

    // (no extra test drawable)

    // (font and fpsText already set above before headless check)

    sf::Clock clock; // used to compute delta time
    // position player in the center
    player.setPosition({400.f, 300.f});

    while (window.isOpen())
    {
        // dt (frame time)
        sf::Time dt = clock.restart();

        // --- Events ---
        while (const auto ev = window.pollEvent())
        {
            if (ev->is<sf::Event::Closed>())
                window.close();

            if (auto key = ev->getIf<sf::Event::KeyPressed>())
            {
                if (key->code == sf::Keyboard::Key::Escape)
                    window.close();
            }
        }

    // --- Update ---
    // player input/update
        player.handleInput();
        player.update(dt);

        // If font loaded show simple FPS
        if (fpsText) {
            float fps = 1.f / std::max(1e-6f, dt.asSeconds());
            fpsText->setString("FPS: " + std::to_string(static_cast<int>(fps + 0.5f)));
        }

    // --- Render ---
    window.clear(sf::Color::Black);
    player.draw(window);
    if (fpsText) window.draw(*fpsText);
    window.display();
    }

    return 0;
}
