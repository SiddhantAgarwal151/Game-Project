#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Triangle with VertexArray");

    // Create a triangle primitive
    sf::VertexArray triangle(sf::Triangles, 3);

    // Define vertices positions
    triangle[0].position = {200.f, 100.f};
    triangle[1].position = {500.f, 100.f};
    triangle[2].position = {350.f, 400.f};

    // Define vertex colors
    triangle[0].color = sf::Color::Red;
    triangle[1].color = sf::Color::Blue;
    triangle[2].color = sf::Color::Green;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(triangle);
        window.display();
    }
}
