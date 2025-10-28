#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <cmath>
#include <string>
#include <iostream>
#include <cstdlib> // for std::rand
#include "CPU.hpp"
#include "ScoreManager.hpp"

constexpr unsigned WINDOW_W = 800;
constexpr unsigned WINDOW_H = 600;
constexpr float paddleSpeed = 400.f;
constexpr float ballRadius = 8.f;

// Function to reset the ball
sf::Vector2f resetBall(sf::CircleShape &ball, int direction = 1) {
    ball.setPosition(WINDOW_W / 2.f - ballRadius, WINDOW_H / 2.f - ballRadius);
    float angle = (std::rand() % 120 - 60) * 3.14159f / 180.f; // -60..60 deg
    float speed = 350.f;
    return sf::Vector2f(std::cos(angle) * speed * direction, std::sin(angle) * speed);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_W, WINDOW_H), "Pong - SFML");
    window.setFramerateLimit(60);

    // Paddles
    const sf::Vector2f paddleSize{10.f, 100.f};
    sf::RectangleShape leftPaddle(paddleSize);
    sf::RectangleShape rightPaddle(paddleSize);
    leftPaddle.setFillColor(sf::Color::White);
    rightPaddle.setFillColor(sf::Color::White);
    leftPaddle.setPosition(30.f, WINDOW_H / 2.f - paddleSize.y / 2.f);
    rightPaddle.setPosition(WINDOW_W - 30.f - paddleSize.x, WINDOW_H / 2.f - paddleSize.y / 2.f);

    // CPU Paddle (slightly slower than player)
    CPU cpu(rightPaddle, paddleSpeed * 0.9f);

    // Ball
    sf::CircleShape ball(ballRadius);
    ball.setFillColor(sf::Color::White);
    sf::Vector2f ballVelocity = resetBall(ball, (std::rand() % 2) ? 1 : -1);

    // Score manager
    ScoreManager scoreManager("highscore.txt");

    // Score display (requires a font file)
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Warning: failed to load 'arial.ttf'. Score will not be shown.\n";
    }

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(36);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(WINDOW_W / 2.f - 40.f, 10.f);

    unsigned scoreLeft = 0, scoreRight = 0;

    // Net line
    sf::RectangleShape net({2.f, static_cast<float>(WINDOW_H)});
    net.setPosition(WINDOW_W / 2.f - 1.f, 0.f);
    net.setFillColor(sf::Color(180, 180, 180, 100));

    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Time dt = clock.restart();
        float dtSec = dt.asSeconds();

        // Event handling
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();
        }

        // Player input (left paddle)
        sf::Vector2f leftPos = leftPaddle.getPosition();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            leftPos.y -= paddleSpeed * dtSec;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            leftPos.y += paddleSpeed * dtSec;
        leftPos.y = std::clamp(leftPos.y, 0.f, static_cast<float>(WINDOW_H) - paddleSize.y);
        leftPaddle.setPosition(leftPos);

        // CPU controls right paddle
        cpu.update(ball, dtSec);

        // Update ball
        ball.move(ballVelocity * dtSec);

        // Top/bottom collision
        if (ball.getPosition().y <= 0.f) {
            ball.setPosition(ball.getPosition().x, 0.f);
            ballVelocity.y = -ballVelocity.y;
        } else if (ball.getPosition().y + ballRadius * 2 >= WINDOW_H) {
            ball.setPosition(ball.getPosition().x, WINDOW_H - ballRadius * 2);
            ballVelocity.y = -ballVelocity.y;
        }

        // Paddle collisions
        sf::FloatRect leftRect = leftPaddle.getGlobalBounds();
        sf::FloatRect rightRect = rightPaddle.getGlobalBounds();
        sf::Vector2f ballCenter = {ball.getPosition().x + ballRadius, ball.getPosition().y + ballRadius};

        auto intersectsPaddle = [&](const sf::FloatRect& rect) {
            float closestX = std::clamp(ballCenter.x, rect.left, rect.left + rect.width);
            float closestY = std::clamp(ballCenter.y, rect.top, rect.top + rect.height);
            float dx = ballCenter.x - closestX;
            float dy = ballCenter.y - closestY;
            return (dx*dx + dy*dy) <= (ballRadius * ballRadius);
        };

        if (intersectsPaddle(leftRect) && ballVelocity.x < 0.f) {
            float hitPos = (ballCenter.y - leftRect.top) / leftRect.height;
            float angle = (hitPos - 0.5f) * (3.14159f / 3.f); // -60..60 deg
            float speed = std::hypot(ballVelocity.x, ballVelocity.y) * 1.05f;
            ballVelocity.x = std::cos(angle) * speed;
            ballVelocity.y = std::sin(angle) * speed;
            if (ballVelocity.x < 0) ballVelocity.x = -ballVelocity.x;
        }

        if (intersectsPaddle(rightRect) && ballVelocity.x > 0.f) {
            float hitPos = (ballCenter.y - rightRect.top) / rightRect.height;
            float angle = (hitPos - 0.5f) * (3.14159f / 3.f);
            float speed = std::hypot(ballVelocity.x, ballVelocity.y) * 1.05f;
            ballVelocity.x = -std::cos(angle) * speed;
            ballVelocity.y = std::sin(angle) * speed;
            if (ballVelocity.x > 0) ballVelocity.x = -ballVelocity.x;
        }

        // Score handling
        if (ball.getPosition().x + ballRadius * 2 < 0.f) {
            ++scoreRight;
            scoreManager.setScore(scoreLeft, scoreRight);
            ballVelocity = resetBall(ball, 1);
        } else if (ball.getPosition().x > WINDOW_W) {
            ++scoreLeft;
            scoreManager.setScore(scoreLeft, scoreRight);
            ballVelocity = resetBall(ball, -1);
        }

        // Update score text
        if (font.getInfo().family.size() > 0) {
            scoreText.setString(
                std::to_string(scoreLeft) + "  -  " + std::to_string(scoreRight) +
                "   High: " + std::to_string(scoreManager.getHighScore())
            );
            sf::FloatRect tb = scoreText.getLocalBounds();
            scoreText.setOrigin(tb.left + tb.width/2.f, tb.top + tb.height/2.f);
            scoreText.setPosition(WINDOW_W / 2.f, 30.f);
        }

        // Draw everything
        window.clear(sf::Color::Black);
        window.draw(net);
        window.draw(leftPaddle);
        window.draw(rightPaddle);
        window.draw(ball);
        if (font.getInfo().family.size() > 0) window.draw(scoreText);
        window.display();
    }

    // Save high score when game exits
    scoreManager.saveHighScore();

    return 0;
}