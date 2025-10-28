#ifndef SCOREMANAGER_HPP
#define SCOREMANAGER_HPP

#include <string>
#include <fstream>
#include <algorithm>

class ScoreManager {
private:
    std::string filename;
    unsigned highScore;
    unsigned currentScore;

public:
    ScoreManager(const std::string &fname) 
        : filename(fname), highScore(0), currentScore(0)
    {
        loadHighScore();
    }

    void loadHighScore() {
        std::ifstream file(filename);
        if (file.is_open()) {
            file >> highScore;
            file.close();
        }
    }

    void saveHighScore() {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << highScore;
            file.close();
        }
    }

    void setScore(unsigned left, unsigned right) {
        currentScore = left;
        if (currentScore > highScore) {
            highScore = currentScore;
        }
    }

    unsigned getHighScore() const {
        return highScore;
    }

    unsigned getCurrentScore() const {
        return currentScore;
    }
};

#endif