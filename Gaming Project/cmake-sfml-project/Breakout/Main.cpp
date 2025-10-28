#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include "Brick.hpp"
#include "ScoreManager.hpp"

constexpr unsigned WINDOW_W = 800;
constexpr unsigned WINDOW_H = 600;
constexpr float paddleSpeed = 500.f;
constexpr float ballRadius = 8.f;
constexpr float initialBallSpeed = 350.f;

// Function to reset the ball
sf::Vector2f resetBall(sf::CircleShape &ball) {
    ball.setPosition(WINDOW_W / 2.f - ballRadius, WINDOW_H / 2.f - ballRadius);
    float angle = (std::rand() % 60 - 30) * 3.14159f / 180.f; // -30..30 deg
    return sf::Vector2f(std::cos(angle) * initialBallSpeed, -std::abs(std::sin(angle) * initialBallSpeed));
}

// Create brick layout
std::vector<Brick> createBricks() {
    std::vector<Brick> bricks;
    constexpr float brickW = 75.f;
    constexpr float brickH = 25.f;
    constexpr float spacing = 5.f;
    constexpr int rows = 8;
    constexpr int cols = 10;
    constexpr float offsetX = (WINDOW_W - (cols * (brickW + spacing) - spacing)) / 2.f;
    constexpr float offsetY = 60.f;

    sf::Color colors[] = {
        sf::Color::Red, sf::Color(255, 140, 0), sf::Color::Yellow,
        sf::Color::Green, sf::Color::Cyan, sf::Color::Blue,
        sf::Color::Magenta, sf::Color(255, 105, 180)
    };

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            float x = offsetX + col * (brickW + spacing);
            float y = offsetY + row * (brickH + spacing);
            int points = (rows - row) * 10;
            bricks.emplace_back(x, y, brickW, brickH, colors[row % 8], points);
        }
    }
    return bricks;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_W, WINDOW_H), "Breakout - SFML");
    window.setFramerateLimit(60);

    // Paddle
    const sf::Vector2f paddleSize{100.f, 15.f};
    sf::RectangleShape paddle(paddleSize);
    paddle.setFillColor(sf::Color::White);
    paddle.setPosition(WINDOW_W / 2.f - paddleSize.x / 2.f, WINDOW_H - 50.f);

    // Ball
    sf::CircleShape ball(ballRadius);
    ball.setFillColor(sf::Color::White);
    sf::Vector2f ballVelocity = resetBall(ball);

    // Bricks
    std::vector<Brick> bricks = createBricks();

    // Score manager
    ScoreManager scoreManager("breakout_highscore.txt");
    unsigned score = 0;
    unsigned lives = 3;
    bool gameOver = false;
    bool gameWon = false;

    // Font and text
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Warning: failed to load 'arial.ttf'. Score will not be shown.\n";
    }

    sf::Text scoreText, livesText, gameOverText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10.f, 10.f);

    livesText.setFont(font);
    livesText.setCharacterSize(24);
    livesText.setFillColor(sf::Color::White);
    livesText.setPosition(WINDOW_W - 150.f, 10.f);

    gameOverText.setFont(font);
    gameOverText.setCharacterSize(48);
    gameOverText.setFillColor(sf::Color::White);

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
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R && (gameOver || gameWon)) {
                // Reset game
                score = 0;
                lives = 3;
                gameOver = false;
                gameWon = false;
                bricks = createBricks();
                ballVelocity = resetBall(ball);
                paddle.setPosition(WINDOW_W / 2.f - paddleSize.x / 2.f, WINDOW_H - 50.f);
            }
        }

        if (!gameOver && !gameWon) {
            // Player input
            sf::Vector2f paddlePos = paddle.getPosition();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                paddlePos.x -= paddleSpeed * dtSec;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                paddlePos.x += paddleSpeed * dtSec;
            paddlePos.x = std::clamp(paddlePos.x, 0.f, static_cast<float>(WINDOW_W) - paddleSize.x);
            paddle.setPosition(paddlePos);

            // Update ball
            ball.move(ballVelocity * dtSec);

            // Wall collisions
            if (ball.getPosition().x <= 0.f) {
                ball.setPosition(0.f, ball.getPosition().y);
                ballVelocity.x = -ballVelocity.x;
            } else if (ball.getPosition().x + ballRadius * 2 >= WINDOW_W) {
                ball.setPosition(WINDOW_W - ballRadius * 2, ball.getPosition().y);
                ballVelocity.x = -ballVelocity.x;
            }

            if (ball.getPosition().y <= 0.f) {
                ball.setPosition(ball.getPosition().x, 0.f);
                ballVelocity.y = -ballVelocity.y;
            }

            // Ball falls below paddle
            if (ball.getPosition().y > WINDOW_H) {
                --lives;
                if (lives == 0) {
                    gameOver = true;
                    scoreManager.setScore(score, 0);
                    scoreManager.saveHighScore();
                } else {
                    ballVelocity = resetBall(ball);
                    paddle.setPosition(WINDOW_W / 2.f - paddleSize.x / 2.f, WINDOW_H - 50.f);
                }
            }

            // Paddle collision
            sf::FloatRect paddleRect = paddle.getGlobalBounds();
            sf::Vector2f ballCenter = {ball.getPosition().x + ballRadius, ball.getPosition().y + ballRadius};

            auto intersectsPaddle = [&](const sf::FloatRect& rect) {
                float closestX = std::clamp(ballCenter.x, rect.left, rect.left + rect.width);
                float closestY = std::clamp(ballCenter.y, rect.top, rect.top + rect.height);
                float dx = ballCenter.x - closestX;
                float dy = ballCenter.y - closestY;
                return (dx*dx + dy*dy) <= (ballRadius * ballRadius);
            };

            if (intersectsPaddle(paddleRect) && ballVelocity.y > 0.f) {
                float hitPos = (ballCenter.x - paddleRect.left) / paddleRect.width;
                float angle = (hitPos - 0.5f) * (2.5f); // -1.25..1.25 radians
                float speed = std::hypot(ballVelocity.x, ballVelocity.y);
                ballVelocity.x = std::sin(angle) * speed;
                ballVelocity.y = -std::abs(std::cos(angle) * speed);
            }

            // Brick collisions
            for (auto &brick : bricks) {
                if (!brick.isDestroyed() && brick.checkCollision(ball, ballRadius, ballVelocity)) {
                    score += brick.getPoints();
                    scoreManager.setScore(score, 0);
                }
            }

            // Check win condition
            bool allDestroyed = true;
            for (const auto &brick : bricks) {
                if (!brick.isDestroyed()) {
                    allDestroyed = false;
                    break;
                }
            }
            if (allDestroyed) {
                gameWon = true;
                scoreManager.setScore(score, 0);
                scoreManager.saveHighScore();
            }
        }

        // Update UI text
        if (font.getInfo().family.size() > 0) {
            scoreText.setString("Score: " + std::to_string(score) + 
                              "  High: " + std::to_string(scoreManager.getHighScore()));
            livesText.setString("Lives: " + std::to_string(lives));

            if (gameOver) {
                gameOverText.setString("Game Over!\nPress R to Restart");
                sf::FloatRect tb = gameOverText.getLocalBounds();
                gameOverText.setOrigin(tb.left + tb.width/2.f, tb.top + tb.height/2.f);
                gameOverText.setPosition(WINDOW_W / 2.f, WINDOW_H / 2.f);
            } else if (gameWon) {
                gameOverText.setString("You Won!\nPress R to Restart");
                sf::FloatRect tb = gameOverText.getLocalBounds();
                gameOverText.setOrigin(tb.left + tb.width/2.f, tb.top + tb.height/2.f);
                gameOverText.setPosition(WINDOW_W / 2.f, WINDOW_H / 2.f);
            }
        }

        // Draw everything
        window.clear(sf::Color::Black);
        
        for (const auto &brick : bricks) {
            if (!brick.isDestroyed())
                brick.draw(window);
        }
        
        window.draw(paddle);
        window.draw(ball);
        
        if (font.getInfo().family.size() > 0) {
            window.draw(scoreText);
            window.draw(livesText);
            if (gameOver || gameWon)
                window.draw(gameOverText);
        }
        
        window.display();
    }

    return 0;
}