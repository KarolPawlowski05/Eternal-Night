#ifndef SCOREMANAGER_H
#define SCOREMANAGER_H

#include <string>
#include <vector>
#include <tuple>

class ScoreManager {
public:
    void saveScore(const std::string& name, int score);
    std::string loadScores();
};

#endif