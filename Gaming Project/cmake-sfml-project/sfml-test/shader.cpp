#include <SFML/Graphics.hpp>
#include <iostream>

constexpr std::string_view fragmentShader = R"(
uniform sampler2D texture;
uniform float time;

void main() {
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
    float pulse = abs(sin(time * 2.0)); // oscillate color
    gl_FragColor = vec4(pixel.r * 0.5, pixel.g * pulse, pixel.b + 0.2, 1.0);
}
)";

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Shader Example");

    if (!sf::Shader::isAvailable()) {
        std::cerr << "Shaders are not supported on this system.\n";
        return -1;
    }

    sf::Texture marioTexture;
    if (!marioTexture.loadFromFile("Mario.png")) {
        std::cerr << "Failed to load Mario.png\n";
        return -1;
    }

    sf::Sprite mario(marioTexture);
    mario.setPosition(200.f, 200.f);

    sf::Shader shader;
    if (!shader.loadFromMemory(fragmentShader.data(), sf::Shader::Fragment)) {
        std::cerr << "Failed to load shader\n";
        return -1;
    }

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float time = clock.getElapsedTime().asSeconds();
        shader.setUniform("time", time);
        shader.setUniform("texture", sf::Shader::CurrentTexture);

        window.clear();
        window.draw(mario, &shader);
        window.display();
    }

    return 0;
}