#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML World Map + Character Example");
    window.setFramerateLimit(60);

    // --- Load character texture ---
    sf::Texture marioTexture;
    if (!marioTexture.loadFromFile("Mario.png")) {
        std::cerr << "Failed to load Mario.png\n";
        return -1;
    }

    sf::Sprite mario(marioTexture);
    mario.setPosition(400.f, 300.f); // starting position
    mario.setScale(0.5f, 0.5f);      // resize sprite if needed

    // --- Create a big world map (tile background) ---
    sf::Texture grassTexture;
    if (!grassTexture.loadFromFile("grass.png")) {
        std::cerr << "Failed to load grass.png\n";
        return -1;
    }

    // Use a big rectangle to represent the world (like a 2D tile field)
    sf::Sprite world(grassTexture);
    world.setScale(5.f, 5.f); // make the texture cover more space visually

    // --- Create a background grid (optional visual aid) ---
    sf::VertexArray grid(sf::Lines);
    const int gridSize = 100;
    for (int x = 0; x <= 2000; x += gridSize) {
        grid.append(sf::Vertex(sf::Vector2f(x, 0), sf::Color(80, 80, 80)));
        grid.append(sf::Vertex(sf::Vector2f(x, 2000), sf::Color(80, 80, 80)));
    }
    for (int y = 0; y <= 2000; y += gridSize) {
        grid.append(sf::Vertex(sf::Vector2f(0, y), sf::Color(80, 80, 80)));
        grid.append(sf::Vertex(sf::Vector2f(2000, y), sf::Color(80, 80, 80)));
    }

    // --- Setup the main view (camera) ---
    sf::View camera(sf::FloatRect(0.f, 0.f, 800.f, 600.f));
    camera.setCenter(mario.getPosition()); // start centered on Mario

    // --- Text for coordinates ---
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load arial.ttf\n";
        return -1;
    }

    sf::Text coordText("", font, 18);
    coordText.setFillColor(sf::Color::White);
    coordText.setPosition(10.f, 10.f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Resize the view dynamically if window changes
            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0.f, 0.f, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            }
        }

        // --- Move character ---
        float moveSpeed = 4.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            mario.move(-moveSpeed, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            mario.move(moveSpeed, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            mario.move(0.f, -moveSpeed);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            mario.move(0.f, moveSpeed);

        // --- Update camera to follow character ---
        camera.setCenter(mario.getPosition());

        // Clamp camera inside world boundaries (so it doesn’t show outside edges)
        sf::Vector2f halfSize = camera.getSize() / 2.f;
        sf::Vector2f center = camera.getCenter();
        float worldWidth = 2000.f;
        float worldHeight = 2000.f;

        if (center.x - halfSize.x < 0)
            center.x = halfSize.x;
        if (center.y - halfSize.y < 0)
            center.y = halfSize.y;
        if (center.x + halfSize.x > worldWidth)
            center.x = worldWidth - halfSize.x;
        if (center.y + halfSize.y > worldHeight)
            center.y = worldHeight - halfSize.y;

        camera.setCenter(center);

        // --- Update coordinate text ---
        coordText.setString(
            "Player world position: (" + std::to_string((int)mario.getPosition().x) + ", "
            + std::to_string((int)mario.getPosition().y) + ")");

        // --- Drawing ---
        window.clear(sf::Color::Black);
        window.setView(camera);
        window.draw(world);
        window.draw(grid);
        window.draw(mario);

        // Draw text overlay in screen space (not affected by camera)
        window.setView(window.getDefaultView());
        window.draw(coordText);

        window.display();
    }

    return 0;
}