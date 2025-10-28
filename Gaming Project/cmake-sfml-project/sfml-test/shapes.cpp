#include <SFML/Graphics.hpp>
#include <array>
#include <cmath>

// Custom ellipse shape class
class EllipseShape : public sf::Shape
{
public:
    explicit EllipseShape(sf::Vector2f radius = {0, 0}) : m_radius(radius)
    {
        update();
    }

    void setRadius(sf::Vector2f radius)
    {
        m_radius = radius;
        update();
    }

    sf::Vector2f getRadius() const { return m_radius; }

    std::size_t getPointCount() const override
    {
        return 60; // higher = smoother ellipse
    }

    sf::Vector2f getPoint(std::size_t index) const override
    {
        static constexpr float pi = 3.141592654f;
        float angle = index * 2 * pi / getPointCount() - pi / 2;
        float x     = std::cos(angle) * m_radius.x;
        float y     = std::sin(angle) * m_radius.y;
        return m_radius + sf::Vector2f(x, y);
    }

private:
    sf::Vector2f m_radius;
};

int main()
{
    // Enable anti-aliasing
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(
        sf::VideoMode({800, 600}),
        "SFML Shapes Tutorial",
        sf::Style::Default,
        settings
    );

    // Rectangle
    sf::RectangleShape rectangle({120.f, 50.f});
    rectangle.setFillColor(sf::Color(100, 250, 50));
    rectangle.setOutlineThickness(5.f);
    rectangle.setOutlineColor(sf::Color::Black);
    rectangle.setPosition(50.f, 50.f);

    // Circle
    sf::CircleShape circle(50.f);
    circle.setFillColor(sf::Color(150, 50, 250));
    circle.setOutlineThickness(5.f);
    circle.setOutlineColor(sf::Color(250, 150, 100));
    circle.setPosition(250.f, 50.f);

    // Triangle (3-point circle)
    sf::CircleShape triangle(80.f, 3);
    triangle.setFillColor(sf::Color::Cyan);
    triangle.setPosition(450.f, 50.f);

    // Convex shape (pentagon-like)
    sf::ConvexShape convex;
    convex.setPointCount(5);
    convex.setPoint(0, {0.f, 0.f});
    convex.setPoint(1, {150.f, 10.f});
    convex.setPoint(2, {120.f, 90.f});
    convex.setPoint(3, {30.f, 100.f});
    convex.setPoint(4, {0.f, 50.f});
    convex.setFillColor(sf::Color::Yellow);
    convex.setPosition(50.f, 250.f);

    // Line with thickness (rectangle)
    sf::RectangleShape thickLine({150.f, 5.f});
    thickLine.rotate(45.f);
    thickLine.setFillColor(sf::Color::Red);
    thickLine.setPosition(300.f, 250.f);

    // Line without thickness (vertices)
    std::array<sf::Vertex, 2> thinLine =
    {
        sf::Vertex(sf::Vector2f(500.f, 250.f), sf::Color::Blue),
        sf::Vertex(sf::Vector2f(650.f, 350.f), sf::Color::Blue)
    };

    // Custom ellipse
    EllipseShape ellipse({100.f, 50.f});
    ellipse.setFillColor(sf::Color(200, 100, 50));
    ellipse.setOutlineThickness(3.f);
    ellipse.setOutlineColor(sf::Color::Black);
    ellipse.setPosition(300.f, 400.f);

    while (window.isOpen())
    {
        sf::Event event{};
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::White);

        window.draw(rectangle);
        window.draw(circle);
        window.draw(triangle);
        window.draw(convex);
        window.draw(thickLine);
        window.draw(thinLine.data(), thinLine.size(), sf::Lines);
        window.draw(ellipse);

        window.display();
    }

    return 0;
}
