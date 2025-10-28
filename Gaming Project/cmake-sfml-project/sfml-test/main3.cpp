#include <SFML/Graphics.hpp>

int main()
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode{800, 600}, "SFML 3.0.2 Tutorial");

   sf::RectangleShape rectangle(sf::Vector2f(100.f, 50.f));
rectangle.setFillColor(sf::Color::Blue);
rectangle.setPosition(350.f, 500.f);
window.draw(rectangle);


    // Main loop
    while (window.isOpen())
    {
        // Event processing
        sf::Event event;
        while (window.pollEvent(event))  // <-- SFML 3.x style
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Clear the window with black
        window.clear(sf::Color::Black);

        // Draw everything
        window.draw(rectangle);

        // Display the contents of the window
        window.display();
    }

    return 0;
}
