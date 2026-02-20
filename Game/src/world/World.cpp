#include "World.hpp"
#include <random>
#include <iostream>

namespace game::world {

World::World(unsigned int width, unsigned int height, float tileSize)
    : m_width(width)
    , m_height(height)
    , m_tileSize(tileSize)
{
    m_worldBounds = sf::FloatRect(
        sf::Vector2f(0.f, 0.f),
        sf::Vector2f(width * tileSize, height * tileSize)
    );
    
    m_tiles.resize(m_height);
    for (auto& row : m_tiles) {
        row.resize(m_width);
    }
}

bool World::loadBackgroundTexture(const std::string& texturePath)
{
    if (!m_backgroundTexture.loadFromFile(texturePath)) {
        std::cerr << "Failed to load background texture: " << texturePath << "\n";
        std::cerr << "Creating fallback background...\n";
        
        // Create simple colored background as fallback
        sf::Image bgImage({100, 100}, sf::Color(34, 139, 34)); // Green
        if (!m_backgroundTexture.loadFromImage(bgImage)) {
            std::cerr << "Failed to create fallback background\n";
            return false;
        }
    } else {
        std::cout << "Background texture loaded: " << texturePath << "\n";
    }
    
    m_backgroundTexture.setRepeated(true);
    m_hasBackground = true;
    buildBackgroundVertices();
    return true;
}

bool World::loadTileset(const std::string& texturePath, const sf::Vector2i& tileSize)
{
    m_tilesetTileSize = tileSize;
    
    if (!m_tilesetTexture.loadFromFile(texturePath)) {
        std::cerr << "Failed to load tileset: " << texturePath << "\n";
        std::cerr << "Creating fallback tileset...\n";
        
        // Create simple placeholder tileset (5 tiles in a row for our 5 tile types)
        int tileCount = 5;
        sf::Image tilesetImage({static_cast<unsigned int>(tileSize.x * tileCount), 
                                static_cast<unsigned int>(tileSize.y)}, 
                               sf::Color::Transparent);
        
        // Add colored tiles for each type
        for (int i = 0; i < tileCount; ++i) {
            sf::Color color = getTileColor(static_cast<TileType>(i));
            
            for (int y = 0; y < tileSize.y; ++y) {
                for (int x = 0; x < tileSize.x; ++x) {
                    tilesetImage.setPixel({static_cast<unsigned int>(i * tileSize.x + x), 
                                          static_cast<unsigned int>(y)}, color);
                }
            }
        }
        
        if (!m_tilesetTexture.loadFromImage(tilesetImage)) {
            std::cerr << "Failed to create fallback tileset\n";
            return false;
        }
    } else {
        std::cout << "Tileset loaded: " << texturePath << "\n";
    }
    
    sf::Vector2u texSize = m_tilesetTexture.getSize();
    m_tilesPerRow = texSize.x / tileSize.x;
    
    std::cout << "Tileset: " << texSize.x << "x" << texSize.y 
              << " | Tile size: " << tileSize.x << "x" << tileSize.y
              << " | Tiles per row: " << m_tilesPerRow << "\n";
    
    m_hasTileset = true;
    buildTileVertices();
    return true;
}

void World::buildBackgroundVertices()
{
    if (!m_hasBackground) return;
    
    m_backgroundVertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    m_backgroundVertices.resize(6); // One quad = 2 triangles = 6 vertices
    
    float worldWidth = m_width * m_tileSize;
    float worldHeight = m_height * m_tileSize;
    
    // Get texture size for proper UV mapping (repeating texture)
    sf::Vector2u texSize = m_backgroundTexture.getSize();
    float uMax = worldWidth / texSize.x;
    float vMax = worldHeight / texSize.y;
    
    // Triangle 1
    m_backgroundVertices[0].position = sf::Vector2f(0.f, 0.f);
    m_backgroundVertices[0].texCoords = sf::Vector2f(0.f, 0.f);
    
    m_backgroundVertices[1].position = sf::Vector2f(worldWidth, 0.f);
    m_backgroundVertices[1].texCoords = sf::Vector2f(uMax, 0.f);
    
    m_backgroundVertices[2].position = sf::Vector2f(0.f, worldHeight);
    m_backgroundVertices[2].texCoords = sf::Vector2f(0.f, vMax);
    
    // Triangle 2
    m_backgroundVertices[3].position = sf::Vector2f(0.f, worldHeight);
    m_backgroundVertices[3].texCoords = sf::Vector2f(0.f, vMax);
    
    m_backgroundVertices[4].position = sf::Vector2f(worldWidth, 0.f);
    m_backgroundVertices[4].texCoords = sf::Vector2f(uMax, 0.f);
    
    m_backgroundVertices[5].position = sf::Vector2f(worldWidth, worldHeight);
    m_backgroundVertices[5].texCoords = sf::Vector2f(uMax, vMax);
}

void World::buildTileVertices()
{
    if (!m_hasTileset) return;
    
    m_tileVertices.setPrimitiveType(sf::PrimitiveType::Triangles);
    m_tileVertices.resize(m_width * m_height * 6);
    
    for (unsigned int y = 0; y < m_height; ++y) {
        for (unsigned int x = 0; x < m_width; ++x) {
            int tileIndex = getTileTextureIndex(m_tiles[y][x].type);
            
            // Calculate tile position in tileset texture
            int tu = tileIndex % m_tilesPerRow;
            int tv = tileIndex / m_tilesPerRow;
            
            // Get vertex pointer for this tile's quad
            sf::Vertex* quad = &m_tileVertices[(x + y * m_width) * 6];
            
            // Position in world
            float px = x * m_tileSize;
            float py = y * m_tileSize;
            
            // Position in tileset texture
            float tx = tu * m_tilesetTileSize.x;
            float ty = tv * m_tilesetTileSize.y;
            
            // Triangle 1
            quad[0].position = sf::Vector2f(px, py);
            quad[0].texCoords = sf::Vector2f(tx, ty);
            
            quad[1].position = sf::Vector2f(px + m_tileSize, py);
            quad[1].texCoords = sf::Vector2f(tx + m_tilesetTileSize.x, ty);
            
            quad[2].position = sf::Vector2f(px, py + m_tileSize);
            quad[2].texCoords = sf::Vector2f(tx, ty + m_tilesetTileSize.y);
            
            // Triangle 2
            quad[3].position = sf::Vector2f(px, py + m_tileSize);
            quad[3].texCoords = sf::Vector2f(tx, ty + m_tilesetTileSize.y);
            
            quad[4].position = sf::Vector2f(px + m_tileSize, py);
            quad[4].texCoords = sf::Vector2f(tx + m_tilesetTileSize.x, ty);
            
            quad[5].position = sf::Vector2f(px + m_tileSize, py + m_tileSize);
            quad[5].texCoords = sf::Vector2f(tx + m_tilesetTileSize.x, ty + m_tilesetTileSize.y);
        }
    }
}

int World::getTileTextureIndex(TileType type) const
{
    // Map tile types to indices in your tileset
    // Assumes tileset has tiles in this order: Grass, Water, Stone, Sand, Wall
    switch (type) {
        case TileType::Grass: return 0;
        case TileType::Water: return 1;
        case TileType::Stone: return 2;
        case TileType::Sand:  return 3;
        case TileType::Wall:  return 4;
        default: return 0;
    }
}

void World::generate()
{
    std::cout << "Generating world: " << m_width << "x" << m_height << " tiles\n";
    
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, 100);
    
    for (unsigned int y = 0; y < m_height; ++y) {
        for (unsigned int x = 0; x < m_width; ++x) {
            Tile& tile = m_tiles[y][x];
            
            // Create borders (walls)
            if (x == 0 || y == 0 || x == m_width - 1 || y == m_height - 1) {
                tile.type = TileType::Wall;
            }
            // Random terrain in the middle
            else {
                int rand = dist(gen);
                if (rand < 5) {
                    tile.type = TileType::Water;
                } else if (rand < 10) {
                    tile.type = TileType::Stone;
                } else if (rand < 15) {
                    tile.type = TileType::Sand;
                } else {
                    tile.type = TileType::Grass;
                }
            }
            
            tile.walkable = isTileWalkable(tile.type);
            tile.shape.setSize(sf::Vector2f(m_tileSize, m_tileSize));
            tile.shape.setPosition(sf::Vector2f(x * m_tileSize, y * m_tileSize));
            tile.shape.setFillColor(getTileColor(tile.type));
            tile.shape.setOutlineColor(sf::Color(0, 0, 0, 50));
            tile.shape.setOutlineThickness(1.f);
        }
    }
    
    // Build tile vertices if tileset is loaded
    if (m_hasTileset) {
        buildTileVertices();
    }
    
    std::cout << "World generated successfully!\n";
}

void World::draw(sf::RenderTarget& target, const sf::FloatRect& viewBounds) const
{
    // Draw background first (if loaded)
    if (m_hasBackground) {
        sf::RenderStates bgStates;
        bgStates.texture = &m_backgroundTexture;
        target.draw(m_backgroundVertices, bgStates);
    }
    
    // Draw tileset (if loaded)
    if (m_hasTileset) {
        sf::RenderStates tileStates;
        tileStates.texture = &m_tilesetTexture;
        target.draw(m_tileVertices, tileStates);
    } else {
        // Fallback: draw colored rectangles
        int startX = std::max(0, static_cast<int>(viewBounds.position.x / m_tileSize) - 1);
        int startY = std::max(0, static_cast<int>(viewBounds.position.y / m_tileSize) - 1);
        int endX = std::min(static_cast<int>(m_width), 
                            static_cast<int>((viewBounds.position.x + viewBounds.size.x) / m_tileSize) + 2);
        int endY = std::min(static_cast<int>(m_height), 
                            static_cast<int>((viewBounds.position.y + viewBounds.size.y) / m_tileSize) + 2);
        
        for (int y = startY; y < endY; ++y) {
            for (int x = startX; x < endX; ++x) {
                target.draw(m_tiles[y][x].shape);
            }
        }
    }
}

bool World::isWalkable(const sf::Vector2f& position) const
{
    int tileX = static_cast<int>(position.x / m_tileSize);
    int tileY = static_cast<int>(position.y / m_tileSize);
    
    if (tileX < 0 || tileX >= static_cast<int>(m_width) || 
        tileY < 0 || tileY >= static_cast<int>(m_height)) {
        return false;
    }
    
    return m_tiles[tileY][tileX].walkable;
}

bool World::checkCollision(const sf::FloatRect& bounds) const
{
    // Check all four corners of the bounding box
    std::vector<sf::Vector2f> corners = {
        sf::Vector2f(bounds.position.x, bounds.position.y),
        sf::Vector2f(bounds.position.x + bounds.size.x, bounds.position.y),
        sf::Vector2f(bounds.position.x, bounds.position.y + bounds.size.y),
        sf::Vector2f(bounds.position.x + bounds.size.x, bounds.position.y + bounds.size.y)
    };
    
    for (const auto& corner : corners) {
        if (!isWalkable(corner)) {
            return true; // Collision detected
        }
    }
    
    return false;
}

sf::Color World::getTileColor(TileType type) const
{
    switch (type) {
        case TileType::Grass: return sf::Color(34, 139, 34);   // Forest green
        case TileType::Water: return sf::Color(65, 105, 225);  // Royal blue
        case TileType::Stone: return sf::Color(128, 128, 128); // Gray
        case TileType::Sand:  return sf::Color(238, 214, 175); // Tan
        case TileType::Wall:  return sf::Color(139, 69, 19);   // Brown
        default: return sf::Color::White;
    }
}

bool World::isTileWalkable(TileType type) const
{
    switch (type) {
        case TileType::Grass:
        case TileType::Sand:
            return true;
        case TileType::Water:
        case TileType::Stone:
        case TileType::Wall:
            return false;
        default:
            return true;
    }
}

} // namespace game::world