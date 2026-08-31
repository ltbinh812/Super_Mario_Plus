#include "DialogueLoader.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>

DialogueSequence DialogueLoader::loadFromFile(const std::string& filePath) {
    DialogueSequence seq;

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[DialogueLoader] Cannot open file: " << filePath << "\n";
        return seq;
    }

    try {
        nlohmann::json j;
        file >> j;

        // Parse ID
        if (j.contains("id") && j["id"].is_string()) {
            seq.id = j["id"].get<std::string>();
        } else {
            std::cerr << "[DialogueLoader] Missing 'id' in " << filePath << "\n";
            return seq;
        }

        // Parse lines
        if (j.contains("lines") && j["lines"].is_array()) {
            for (const auto& lineJson : j["lines"]) {
                DialogueLine line;

                if (lineJson.contains("speaker") && lineJson["speaker"].is_string()) {
                    line.speakerName = lineJson["speaker"].get<std::string>();
                }
                if (lineJson.contains("text") && lineJson["text"].is_string()) {
                    line.text = lineJson["text"].get<std::string>();
                }
                if (lineJson.contains("portrait") && lineJson["portrait"].is_string()) {
                    line.portraitKey = lineJson["portrait"].get<std::string>();
                }
                if (lineJson.contains("textSpeed") && lineJson["textSpeed"].is_number()) {
                    line.textSpeed = lineJson["textSpeed"].get<float>();
                }

                seq.lines.push_back(std::move(line));
            }
        }

        std::cout << "[DialogueLoader] Loaded '" << seq.id << "' with "
                  << seq.lines.size() << " lines from " << filePath << "\n";

    } catch (const nlohmann::json::exception& e) {
        std::cerr << "[DialogueLoader] JSON parse error in " << filePath
                  << ": " << e.what() << "\n";
    }

    return seq;
}
