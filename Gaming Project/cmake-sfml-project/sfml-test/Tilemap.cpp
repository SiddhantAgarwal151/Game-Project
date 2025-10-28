#include <SFML/Graphics.hpp>
#include <array>

class TileMap : public sf::Drawable, public sf::Transformable {
public:
    bool load(const std::string& tileset, sf::Vector2u tileSize, const int* tiles, unsigned width, unsigned height) {
        if (!m_tileset.loadFromFile(tileset))
            return false;

        m_vertices.setPrimitiveType(sf::PrimitiveType::Triangles);
        m_vertices.resize(width * height * 6);

        for (unsigned i = 0; i < width; ++i) {
            for (unsigned j = 0; j < height; ++j) {
                int tileNumber = tiles[i + j * width];
                int tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
                int tv = tileNumber / (m_tileset.getSize().x / tileSize.x);

                sf::Vertex* quad = &m_vertices[(i + j * width) * 6];

                quad[0].position = {i * tileSize.x, j * tileSize.y};
                quad[1].position = {(i + 1) * tileSize.x, j * tileSize.y};
                quad[2].position = {i * tileSize.x, (j + 1) * tileSize.y};
                quad[3].position = {i * tileSize.x, (j + 1) * tileSize.y};
                quad[4].position = {(i + 1) * tileSize.x, j * tileSize.y};
                quad[5].position = {(i + 1) * tileSize.x, (j + 1) * tileSize.y};

                quad[0].texCoords = {tu * tileSize.x, tv * tileSize.y};
                quad[1].texCoords = {(tu + 1) * tileSize.x, tv * tileSize.y};
                quad[2].texCoords = {tu * tileSize.x, (tv + 1) * tileSize.y};
                quad[3].texCoords = {tu * tileSize.x, (tv + 1) * tileSize.y};
                quad[4].texCoords = {(tu + 1) * tileSize.x, tv * tileSize.y};
                quad[5].texCoords = {(tu + 1) * tileSize.x, (tv + 1) * tileSize.y};
            }
        }
        return true;
    }

private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        states.texture = &m_tileset;
        target.draw(m_vertices, states);
    }

    sf::VertexArray m_vertices;
    sf::Texture m_tileset;
};

int main() {
    sf::RenderWindow window(sf::VideoMode(512, 256), "Tilemap");

    constexpr std::array<int, 128> level = {
        0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,
        0,1,1,1,1,1,1,0,0,0,0,2,0,0,0,0,
        1,1,0,0,0,0,0,0,3,3,3,3,3,3,3,3,
        0,1,0,0,2,0,3,3,3,0,1,1,1,0,0,0,
        0,1,1,0,3,3,3,0,0,0,1,1,1,2,0,0,
        0,0,1,0,3,0,2,2,0,0,1,1,1,1,2,0,
        2,0,1,0,3,0,2,2,2,0,1,1,1,1,1,1,
        0,0,1,0,3,2,2,2,0,0,0,0,1,1,1,1
    };

    TileMap map;
    if (!map.load("vertex-array-tilemap-tileset.png", {32, 32}, level.data(), 16, 8))
        return -1;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(map);
        window.display();
    }

}