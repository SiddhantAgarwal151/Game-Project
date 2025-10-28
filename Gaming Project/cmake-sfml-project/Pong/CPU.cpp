// CPU.cpp
#include "CPU.hpp"

CPU::CPU(sf::RectangleShape& paddle, float speed)
    : m_paddle(paddle), m_speed(speed) {}

void CPU::update(const sf::CircleShape& ball, float dtSec) {
    float paddleY = m_paddle.getPosition().y + m_paddle.getSize().y / 2.f;
    float ballY = ball.getPosition().y + ball.getRadius();

    float diff = ballY - paddleY;
    float move = std::clamp(diff, -m_speed * dtSec, m_speed * dtSec);

    sf::Vector2f pos = m_paddle.getPosition();
    pos.y += move;
    pos.y = std::clamp(pos.y, 0.f, 600.f - m_paddle.getSize().y);

    m_paddle.setPosition(pos);
}
