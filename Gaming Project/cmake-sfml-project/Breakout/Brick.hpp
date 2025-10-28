#ifndef BRICK_HPP
#define BRICK_HPP

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>

class Brick {
private:
    sf::RectangleShape shape;
    bool destroyed;
    int points;

public:
    Brick(float x, float y, float width, float height, sf::Color color, int pts)
        : destroyed(false), points(pts)
    {
        shape.setSize(sf::Vector2f(width, height));
        shape.setPosition(x, y);
        shape.setFillColor(color);
        shape.setOutlineColor(sf::Color(0, 0, 0, 100));
        shape.setOutlineThickness(1.f);
    }

    bool checkCollision(sf::CircleShape &ball, float ballRadius, sf::Vector2f &ballVelocity) {
        if (destroyed) return false;

        sf::FloatRect brickRect = shape.getGlobalBounds();
        sf::Vector2f ballCenter = {
            ball.getPosition().x + ballRadius,
            ball.getPosition().y + ballRadius
        };

        // Check if ball center is near brick
        float closestX = std::clamp(ballCenter.x, brickRect.left, brickRect.left + brickRect.width);
        float closestY = std::clamp(ballCenter.y, brickRect.top, brickRect.top + brickRect.height);
        float dx = ballCenter.x - closestX;
        float dy = ballCenter.y - closestY;

        if ((dx*dx + dy*dy) <= (ballRadius * ballRadius)) {
            destroyed = true;

            // Determine collision side
            float overlapLeft = (ballCenter.x + ballRadius) - brickRect.left;
            float overlapRight = (brickRect.left + brickRect.width) - (ballCenter.x - ballRadius);
            float overlapTop = (ballCenter.y + ballRadius) - brickRect.top;
            float overlapBottom = (brickRect.top + brickRect.height) - (ballCenter.y - ballRadius);

            float minOverlap = std::min({overlapLeft, overlapRight, overlapTop, overlapBottom});

            // Bounce based on collision side
            if (minOverlap == overlapLeft || minOverlap == overlapRight) {
                ballVelocity.x = -ballVelocity.x;
            } else {
                ballVelocity.y = -ballVelocity.y;
            }

            return true;
        }

        return false;
    }

    void draw(sf::RenderWindow &window) const {
        if (!destroyed)
            window.draw(shape);
    }

    bool isDestroyed() const { return destroyed; }
    int getPoints() const { return points; }
};

#endif