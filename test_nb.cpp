#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"
#include <string>
#include <vector>
#include <map>

using json = nlohmann::json;

struct NeighbourInfo {
    std::string levelName;
    int worldX;
    int worldY;
    int width;
    int height;
};

std::map<std::string, std::vector<NeighbourInfo>> currentNeighbours;

void loadLevel(const std::string& levelName) {
    std::ifstream f("assets/maps/map03/world03.ldtk");
    json j = json::parse(f);
    json targetLevel = nullptr;
    for (const auto& lvl : j["levels"]) {
        if (lvl["identifier"] == levelName) { targetLevel = lvl; break; }
    }
    currentNeighbours.clear();
    for (const auto& nb : targetLevel["__neighbours"]) {
        std::string dir = nb["dir"];
        std::string levelIid = nb["levelIid"];
        for (const auto& lvl : j["levels"]) {
            if (lvl["iid"] == levelIid) {
                NeighbourInfo info;
                info.levelName = lvl["identifier"];
                info.worldX = lvl["worldX"];
                info.worldY = lvl["worldY"];
                info.width = lvl["pxWid"];
                info.height = lvl["pxHei"];
                currentNeighbours[dir].push_back(info);
                break;
            }
        }
    }
}

std::string GetNeighbour(const std::string& dir, float globalX, float globalY) {
    float scale = 2.0f;
    float margin = 8.0f;
    for (const auto& nb : currentNeighbours[dir]) {
        float nbWorldX = nb.worldX * scale;
        float nbWorldY = nb.worldY * scale;
        float nbWidth = nb.width * scale;
        float nbHeight = nb.height * scale;
        if (dir == "e" || dir == "w") {
            if (globalY >= nbWorldY - margin && globalY <= nbWorldY + nbHeight + margin) return nb.levelName;
        } else if (dir == "n" || dir == "s") {
            if (globalX >= nbWorldX - margin && globalX <= nbWorldX + nbWidth + margin) return nb.levelName;
        }
    }
    return "";
}

int main() {
    loadLevel("Your_typical_2D_platformer");
    std::cout << "East nb: " << GetNeighbour("e", 1696.0f, 288.0f) << std::endl;
    std::cout << "East nb (ground): " << GetNeighbour("e", 1696.0f, 640.0f) << std::endl;
    return 0;
}
