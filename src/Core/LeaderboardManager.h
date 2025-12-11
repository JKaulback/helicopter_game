#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>

struct LeaderboardEntry {
    std::string name;
    int score;
};

class LeaderboardManager {
public:
    LeaderboardManager(const std::string& filename = "leaderboard.csv");
    
    void Load();
    void Save();
    bool IsHighScore(int score) const;
    void AddEntry(const std::string& name, int score);
    const std::vector<LeaderboardEntry>& GetEntries() const;

private:
    std::string filename;
    std::vector<LeaderboardEntry> entries;
    const size_t MaxEntries = 5;
};
