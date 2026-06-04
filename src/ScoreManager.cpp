#include "ScoreManager.h"
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <ctime>

namespace fs = std::filesystem;

void ScoreManager::saveScore(const std::string &name, int score) {
    fs::create_directories("assets/scores");

    std::ofstream outFile("assets/scores/scores.txt", std::ios::app);
    if (outFile.is_open()) {
        time_t t = time(nullptr);
        char dateStr[11];
        strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", localtime(&t));

        outFile << score << "|" << name << "|" << dateStr << "\n";
        outFile.close();
    }
}

std::string ScoreManager::loadScores() {
    std::vector<std::tuple<int, std::string, std::string>> scores;
    std::ifstream inFile("assets/scores/scores.txt");

    if (inFile.is_open()) {
        std::string line;
        while (std::getline(inFile, line)) {
            size_t p1 = line.find('|');
            size_t p2 = line.rfind('|');
            if (p1 != std::string::npos && p2 != std::string::npos && p1 != p2) {
                int sc = std::stoi(line.substr(0, p1));
                std::string nm = line.substr(p1 + 1, p2 - p1 - 1);
                std::string dt = line.substr(p2 + 1);
                scores.push_back({sc, nm, dt});
            }
        }
        inFile.close();
    }

    // Sortuj malejąco po wyniku
    std::sort(scores.begin(), scores.end(), [](const auto& a, const auto& b) {
        return std::get<0>(a) > std::get<0>(b);
    });

    // Zbuduj string do wyświetlenia
    std::string result = "";
    int count = std::min((int)scores.size(), 10);
    for (int i = 0; i < count; i++) {
        result += std::to_string(i + 1) + ".  " +
                  std::get<1>(scores[i]) + "     " +
                  std::to_string(std::get<0>(scores[i])) + "     " +
                  std::get<2>(scores[i]) + "\n\n";
    }

    if (result.empty()) result = "No scores yet!";

    return result;
}