#pragma once
#include <string>

class ScoreManager {
public:
    ScoreManager(const std::string& filename);

    void setScore(unsigned left, unsigned right);
    unsigned getLeftScore() const;
    unsigned getRightScore() const;
    unsigned getHighScore() const;
    void saveHighScore() const;

private:
    unsigned leftScore = 0;
    unsigned rightScore = 0;
    unsigned highScore = 0;
    std::string file;

    void loadHighScore();
};
