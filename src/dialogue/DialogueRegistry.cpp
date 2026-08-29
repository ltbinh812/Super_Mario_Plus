#include "DialogueRegistry.h"
#include "DialogueLoader.h"
#include <iostream>

DialogueRegistry& DialogueRegistry::getInstance() {
    static DialogueRegistry instance;
    return instance;
}

void DialogueRegistry::loadFromFile(const std::string& filePath) {
    DialogueSequence seq = DialogueLoader::loadFromFile(filePath);

    if (seq.id.empty()) {
        std::cerr << "[DialogueRegistry] Failed to load or empty ID from: " << filePath << "\n";
        return;
    }

    // Chỉ cache nếu ID chưa tồn tại (tránh load trùng)
    if (sequences.find(seq.id) == sequences.end()) {
        std::string id = seq.id; // Copy ID trước khi move
        sequences[id] = std::move(seq);
        std::cout << "[DialogueRegistry] Cached dialogue: " << id << "\n";
    }
}

const DialogueSequence* DialogueRegistry::get(const std::string& id) const {
    auto it = sequences.find(id);
    if (it != sequences.end()) {
        return &(it->second);
    }
    return nullptr;
}

bool DialogueRegistry::has(const std::string& id) const {
    return sequences.find(id) != sequences.end();
}
