#include "LeaderboardManager.h"
#include <iostream>

LeaderboardManager::LeaderboardManager(const std::string& filename) : filename(filename) {
    Load();
}

void LeaderboardManager::Load() {
    entries.clear();
    std::ifstream file(filename);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string name;
        std::string scoreStr;
        
        if (std::getline(ss, name, ',') && std::getline(ss, scoreStr)) {
            try {
                int score = std::stoi(scoreStr);
                entries.push_back({name, score});
            } catch (...) {
                // Ignore malformed lines
            }
        }
    }
    
    // Sort just in case the file was modified manually
    std::sort(entries.begin(), entries.end(), [](const LeaderboardEntry& a, const LeaderboardEntry& b) {
        return a.score > b.score;
    });
}

void LeaderboardManager::Save() {
    std::ofstream file(filename);
    if (!file.is_open()) return;

    for (const auto& entry : entries) {
        file << entry.name << "," << entry.score << "\n";
    }
}

bool LeaderboardManager::IsHighScore(int score) const {
    if (entries.size() < MaxEntries) return true;
    return score > entries.back().score;
}

void LeaderboardManager::AddEntry(const std::string& name, int score) {
    entries.push_back({name, score});
    std::sort(entries.begin(), entries.end(), [](const LeaderboardEntry& a, const LeaderboardEntry& b) {
        return a.score > b.score;
    });
    
    if (entries.size() > MaxEntries) {
        entries.resize(MaxEntries);
    }
    Save();
}

const std::vector<LeaderboardEntry>& LeaderboardManager::GetEntries() const {
    return entries;
}
