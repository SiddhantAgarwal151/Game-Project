#include "ScoreManager.hpp"
#include <fstream>
#include <algorithm>
#include <iostream>

ScoreManager::ScoreManager(const std::string& filename) : file(filename) {
    loadHighScore();
}

void ScoreManager::loadHighScore() {
    std::ifstream in(file);
    if (in.is_open()) {
        in >> highScore;
        in.close();
    } else {
        highScore = 0;
    }
}

void ScoreManager::setScore(unsigned left, unsigned right) {
    leftScore = left;
    rightScore = right;
    highScore = std::max({highScore, leftScore, rightScore});
}

unsigned ScoreManager::getLeftScore() const { return leftScore; }
unsigned ScoreManager::getRightScore() const { return rightScore; }
unsigned ScoreManager::getHighScore() const { return highScore; }

void ScoreManager::saveHighScore() const {
    std::ofstream out(file);
    if (out.is_open()) {
        out << highScore;
        out.close();
    } else {
        std::cerr << "Error: cannot save high score to " << file << "\n";
    }
}
