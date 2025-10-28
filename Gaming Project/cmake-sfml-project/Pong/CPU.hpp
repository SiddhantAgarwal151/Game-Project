// CPU.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm> // for std::clamp

class CPU {
public:
    CPU(sf::RectangleShape& paddle, float speed);
    void update(const sf::CircleShape& ball, float dtSec);

private:
    sf::RectangleShape& m_paddle;
    float m_speed;
};
