#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

// -------------------------------------------------------------
// AudioExample: Demonstrates proper usage of sf::Sound and sf::Music
// -------------------------------------------------------------
// - Background music (streamed via sf::Music)
// - Jump sound effect (buffered via sf::Sound)
// - Keyboard input to trigger sounds
// -------------------------------------------------------------

int main() {
    // Create a simple window (for visual feedback)
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Audio Example");
    window.setFramerateLimit(60);

    // -----------------------------
    // Load and set up background music
    // -----------------------------
    sf::Music music;
    if (!music.openFromFile("theme.ogg")) {  // Must remain accessible
        std::cerr << "Error: Could not open music.ogg" << std::endl;
        return -1;
    }

    // Configure music playback
    music.setLoop(true);
    music.setVolume(60.f);   // 0–100
    music.play();            // Start streaming

    // -----------------------------
    // Load and set up sound effect
    // -----------------------------
    sf::SoundBuffer jumpBuffer;
    if (!jumpBuffer.loadFromFile("jump.wav")) {
        std::cerr << "Error: Could not load jump.wav" << std::endl;
        return -1;
    }

    // Attach the buffer to a sound object
    sf::Sound jumpSound;
    jumpSound.setBuffer(jumpBuffer);
    jumpSound.setVolume(100.f);
    jumpSound.setPitch(1.f); // Normal pitch

    // -----------------------------
    // Example sprite (Mario)
    // -----------------------------
    sf::Texture marioTexture;
    if (!marioTexture.loadFromFile("Mario.png")) {
        std::cerr << "Error: Could not load Mario.png" << std::endl;
        return -1;
    }

    sf::Sprite mario(marioTexture);
    mario.setPosition(300.f, 300.f);
    mario.setScale(2.f, 2.f);

    // -----------------------------
    // Main loop
    // -----------------------------
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // --- Movement and jump control ---
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            mario.move(-4.f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            mario.move(4.f, 0.f);

        // Play jump sound when pressing Space (if not already playing)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            if (jumpSound.getStatus() != sf::Sound::Playing) {
                jumpSound.play(); // short buffered sound
                std::cout << "Jump sound played\n";
            }
        }

        // -----------------------------
        // Render everything
        // -----------------------------
        window.clear(sf::Color::Black);
        window.draw(mario);
        window.display();
    }

    // -----------------------------
    // Cleanup happens automatically:
    // - SoundBuffer and Music release OpenAL resources
    // - No explicit alDeleteBuffers needed
    // -----------------------------

    return 0;
}
