#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class Node {
public:
    void setTransform(const sf::Transform& transform) { m_transform = transform; }
    void addChild(std::unique_ptr<Node> child) { m_children.push_back(std::move(child)); }

    void draw(sf::RenderTarget& target, const sf::Transform& parentTransform) const {
        sf::Transform combined = parentTransform * m_transform;
        onDraw(target, combined);

        for (auto& child : m_children)
            child->draw(target, combined);
    }

protected:
    virtual void onDraw(sf::RenderTarget& target, const sf::Transform& transform) const = 0;

private:
    sf::Transform m_transform;
    std::vector<std::unique_ptr<Node>> m_children;
};

// SpriteNode: draws a sprite
class SpriteNode : public Node {
public:
    explicit SpriteNode(const sf::Texture& texture) {
        m_sprite.setTexture(texture);
        m_sprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);
    }

protected:
    void onDraw(sf::RenderTarget& target, const sf::Transform& transform) const override {
        target.draw(m_sprite, transform);
    }

private:
    sf::Sprite m_sprite;
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Mario Scene Graph");

    // Load Mario texture
    sf::Texture marioTexture;
    if (!marioTexture.loadFromFile("Mario.png"))
        return -1;

    sf::Sprite mario(marioTexture);
    mario.setPosition(200.f, 200.f); // starting position

    // Child object (a red circle for demo, could be another sprite like a hat)
    sf::CircleShape child(20.f);
    child.setFillColor(sf::Color::Red);
    child.setPosition(50.f, -20.f); // relative to Mario

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // --- Keyboard input to move Mario ---
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            mario.move(-2.f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            mario.move(2.f, 0.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            mario.move(0.f, -2.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            mario.move(0.f, 2.f);

        // --- Clear + draw ---
        window.clear(sf::Color::Black);

        // Draw Mario
        window.draw(mario);

        // Apply Mario’s transform to the child
        sf::RenderStates states;
        states.transform = mario.getTransform();
        window.draw(child, states);

        window.display();
    }
}