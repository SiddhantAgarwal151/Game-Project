#include <SFML/Graphics.hpp>
#include <iostream>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Text Tutorial");

    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))  // SFML 2.5.1 uses loadFromFile
    {
        std::cerr << "Error: Could not load font!" << std::endl;
        return 1;
    }

    sf::Text hello;
    hello.setFont(font);
    hello.setString("Hello, SFML!");
    hello.setCharacterSize(32);
    hello.setFillColor(sf::Color::Red);
    hello.setStyle(sf::Text::Bold | sf::Text::Underlined);
    hello.setPosition({50.f, 50.f});

    sf::Text unicode;
    unicode.setFont(font);
    unicode.setString(L"שלום עולם");
    unicode.setCharacterSize(32);
    unicode.setFillColor(sf::Color::Blue);
    unicode.setPosition({50.f, 120.f});

    sf::Text rotated;
    rotated.setFont(font);
    rotated.setString("Rotated Text");
    rotated.setCharacterSize(28);
    rotated.setFillColor(sf::Color::Green);
    rotated.setPosition({400.f, 300.f});
    rotated.setRotation(45.f);
    rotated.setScale({1.5f, 1.5f});

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);
        window.draw(hello);
        window.draw(unicode);
        window.draw(rotated);
        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp>