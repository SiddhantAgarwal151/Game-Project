#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <cmath>
#include <thread>
#include <atomic>

// Move the listener in a circular path around the sound source.
void moveListenerCircle(float radius, float speed, float timeSeconds)
{
    float x = std::cos(speed * timeSeconds) * radius;
    float z = std::sin(speed * timeSeconds) * radius;

    sf::Listener::setPosition(x, 0.f, z);

    // Make the listener face the origin (where the sound source is)
    sf::Vector3f direction(-x, 0.f, -z);
    if (direction.x == 0 && direction.y == 0 && direction.z == 0)
        direction = {0.f, 0.f, -1.f};

    sf::Listener::setDirection(direction);
}

int main()
{
    std::cout << "=== SFML Spatial Audio Demo ===\n";
    std::cout << "Sound source is fixed at (0, 0, 0). Listener orbits around it.\n";
    std::cout << "Press Enter to stop.\n\n";

    // Load a MONO sound (stereo files won’t spatialize)
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile("staircase-evening-mono.wav")) {
        std::cerr << "Error: failed to load sound_mono.wav (ensure the file exists and is mono)\n";
        return 1;
    }

    if (buffer.getChannelCount() != 1) {
        std::cerr << "Warning: this file has " << buffer.getChannelCount()
                  << " channels. Spatialization requires mono audio.\n";
    }

    // Create the sound source
    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.setPosition(0.f, 0.f, 0.f);  // sound at origin
    sound.setLoop(true);
    sound.setMinDistance(5.f);
    sound.setAttenuation(5.f);
    sound.play();

    // Configure listener
    sf::Listener::setUpVector({0.f, 1.f, 0.f});
    sf::Listener::setGlobalVolume(100.f);

    // Flag to signal when to stop
    std::atomic<bool> stopFlag{false};

    // Separate thread to detect Enter key
    std::thread inputThread([&stopFlag]() {
        std::cin.get();
        stopFlag = true;
    });

    // Move listener in a circular orbit
    const float radius = 10.f;
    const float speed = 1.0f;
    sf::Clock clock;

    while (!stopFlag && sound.getStatus() == sf::Sound::Playing) {
        float t = clock.getElapsedTime().asSeconds();
        moveListenerCircle(radius, speed, t);
        sf::sleep(sf::milliseconds(25));
    }

    if (inputThread.joinable())
        inputThread.join();

    std::cout << "Exiting spatial audio demo.\n";
    return 0;
}