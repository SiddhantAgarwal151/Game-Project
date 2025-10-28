#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

class ParticleSystem : public sf::Drawable, public sf::Transformable {
public:
    ParticleSystem(unsigned int count) 
        : m_particles(count), m_vertices(sf::PrimitiveType::Points, count) {}

    void setEmitter(sf::Vector2f position) { m_emitter = position; }

    void update(sf::Time elapsed) {
        for (std::size_t i = 0; i < m_particles.size(); ++i) {
            Particle& p = m_particles[i];
            p.lifetime -= elapsed;

            if (p.lifetime <= sf::Time::Zero)
                resetParticle(i);

            m_vertices[i].position += p.velocity * elapsed.asSeconds();

            float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
            m_vertices[i].color.a = static_cast<sf::Uint8>(ratio * 255);
        }
    }

private:
    struct Particle {
        sf::Vector2f velocity;
        sf::Time lifetime;
    };

    void resetParticle(std::size_t i) {
        static std::random_device rd;
        static std::mt19937 rng(rd());

        std::uniform_real_distribution<float> angleDist(0.f, 360.f);
        std::uniform_real_distribution<float> speedDist(50.f, 100.f);
        std::uniform_int_distribution<int> lifeDist(1000, 3000);

        float angle = angleDist(rng) * 3.14159f / 180.f;
        float speed = speedDist(rng);
        m_particles[i].velocity = {std::cos(angle) * speed, std::sin(angle) * speed};
        m_particles[i].lifetime = sf::milliseconds(lifeDist(rng));
        m_vertices[i].position = m_emitter;
        m_vertices[i].color = sf::Color::White;
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        target.draw(m_vertices, states);
    }

    std::vector<Particle> m_particles;
    sf::VertexArray m_vertices;
    sf::Time m_lifetime{sf::seconds(3)};
    sf::Vector2f m_emitter;
};

int main() {
    sf::RenderWindow window(sf::VideoMode(512, 256), "Particles");
    ParticleSystem particles(1000);

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }



        sf::Vector2i mouse = sf::Mouse::getPosition(window);
        particles.setEmitter(window.mapPixelToCoords(mouse));

        sf::Time elapsed = clock.restart();
        particles.update(elapsed);

        window.clear();
        window.draw(particles);
        window.display();
    }
}