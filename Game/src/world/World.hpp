#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <string>

namespace game::world {

enum class TileType {
    Grass,
    Water,
    Stone,
    Sand,
    Wall
};

struct Tile {
    TileType type;
    sf::RectangleShape shape;
    bool walkable;
};

class World {
public:
    World(unsigned int width, unsigned int height, float tileSize = 32.f);
    
    void generate(); // Generate a simple world
    void draw(sf::RenderTarget& target, const sf::FloatRect& viewBounds) const;
    
    bool isWalkable(const sf::Vector2f& position) const;
    sf::FloatRect getWorldBounds() const { return m_worldBounds; }
    
    // Collision check
    bool checkCollision(const sf::FloatRect& bounds) const;
    
    // Texture loading (for Aseprite exports)
    bool loadBackgroundTexture(const std::string& texturePath);
    bool loadTileset(const std::string& texturePath, const sf::Vector2i& tileSize);
    
private:
    unsigned int m_width;
    unsigned int m_height;
    float m_tileSize;
    
    std::vector<std::vector<Tile>> m_tiles;
    sf::FloatRect m_worldBounds;
    
    // Texture rendering
    sf::Texture m_backgroundTexture;
    sf::Texture m_tilesetTexture;
    bool m_hasBackground = false;
    bool m_hasTileset = false;
    
    mutable sf::VertexArray m_backgroundVertices;
    mutable sf::VertexArray m_tileVertices;
    
    sf::Vector2i m_tilesetTileSize{32, 32};
    int m_tilesPerRow = 0;
    
    void buildBackgroundVertices();
    void buildTileVertices();
    
    sf::Color getTileColor(TileType type) const;
    bool isTileWalkable(TileType type) const;
    int getTileTextureIndex(TileType type) const;
};

} // namespace game::world