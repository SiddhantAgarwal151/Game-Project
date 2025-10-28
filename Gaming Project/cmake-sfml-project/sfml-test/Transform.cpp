#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(600, 400), "Transforms Example");

    sf::CircleShape shape(50.f);
    shape.setFillColor(sf::Color::Blue);

    // Apply transformations
    shape.setPosition({200.f, 150.f});       // Move to (200,150)
    shape.setRotation(45.f);                 // Rotate 45 degrees
    shape.setScale({1.5f, 0.5f});            // Stretch x1.5 horizontally, shrink y0.5 vertically
    shape.setOrigin(shape.getLocalBounds().width / 2.f, 
                    shape.getLocalBounds().height / 2.f); // Center origin

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);
        window.draw(shape);
        window.display();
    }
}