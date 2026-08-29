#include "UIAtlasAnimator.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <algorithm>

using json = nlohmann::json;

UIAtlasAnimator::UIAtlasAnimator(const std::string& texturePath, const std::string& jsonPath, float fps) 
    : currentFrame_(0), timer_(0.0f), isPlaying_(false), isFinished_(false) {
    
    frameTime_ = 1.0f / fps;
    texture_ = LoadTexture(texturePath.c_str());

    std::ifstream file(jsonPath);
    if (!file.is_open()) {
        std::cerr << "[UIAtlasAnimator] Failed to open json: " << jsonPath << std::endl;
        return;
    }

    try {
        json j;
        file >> j;
        
        const auto& framesObj = j["frames"];
        
        // Since frames in TexturePacker JSON (hash mode) might not be ordered automatically in C++,
        // we should sort the keys or assume they are ordered if we extract them properly.
        // The keys are "0.png", "1.png", ... "10.png".
        std::vector<std::pair<int, AtlasFrame>> sortedFrames;

        for (auto it = framesObj.begin(); it != framesObj.end(); ++it) {
            std::string key = it.key(); // e.g. "0.png"
            // Extract the number
            int index = 0;
            size_t dotPos = key.find(".png");
            if (dotPos != std::string::npos) {
                index = std::stoi(key.substr(0, dotPos));
            }

            AtlasFrame frame;
            auto f = it.value()["frame"];
            frame.frame = { (float)f["x"], (float)f["y"], (float)f["w"], (float)f["h"] };
            
            auto sss = it.value()["spriteSourceSize"];
            frame.spriteSourceSize = { (float)sss["x"], (float)sss["y"], (float)sss["w"], (float)sss["h"] };
            
            auto ss = it.value()["sourceSize"];
            frame.sourceSize = { (float)ss["w"], (float)ss["h"] };

            sortedFrames.push_back({index, frame});
        }

        // Sort by index to ensure proper playback order
        std::sort(sortedFrames.begin(), sortedFrames.end(), [](const auto& a, const auto& b) {
            return a.first < b.first;
        });

        for (const auto& pair : sortedFrames) {
            frames_.push_back(pair.second);
        }

    } catch (const std::exception& e) {
        std::cerr << "[UIAtlasAnimator] Error parsing json: " << e.what() << std::endl;
    }
}

UIAtlasAnimator::~UIAtlasAnimator() {
    UnloadTexture(texture_);
}

void UIAtlasAnimator::Play() {
    isPlaying_ = true;
    isFinished_ = false;
    currentFrame_ = 0;
    timer_ = 0.0f;
}

void UIAtlasAnimator::Stop() {
    isPlaying_ = false;
}

void UIAtlasAnimator::Reset() {
    currentFrame_ = 0;
    timer_ = 0.0f;
    isPlaying_ = false;
    isFinished_ = false;
}

void UIAtlasAnimator::Update(float dt) {
    if (!isPlaying_ || frames_.empty()) return;

    timer_ += dt;
    if (timer_ >= frameTime_) {
        timer_ -= frameTime_;
        currentFrame_++;
        
        if (currentFrame_ >= frames_.size()) {
            currentFrame_ = frames_.size() - 1;
            isPlaying_ = false;
            isFinished_ = true;
        }
    }
}

void UIAtlasAnimator::Render(Vector2 position, float scale) const {
    RenderFrame(currentFrame_, position, scale);
}

void UIAtlasAnimator::RenderFrame(int frameIndex, Vector2 position, float scale) const {
    if (frames_.empty() || frameIndex < 0 || frameIndex >= frames_.size()) return;

    const AtlasFrame& frame = frames_[frameIndex];
    
    // We want to center the animation based on its original source size.
    // The 'position' is considered the center point.
    // To do this, we calculate the top-left of the original image, then add spriteSourceSize offset.
    
    float originalW = frame.sourceSize.x * scale;
    float originalH = frame.sourceSize.y * scale;
    
    float startX = position.x - originalW / 2.0f;
    float startY = position.y - originalH / 2.0f;
    
    Vector2 drawPos = {
        startX + frame.spriteSourceSize.x * scale,
        startY + frame.spriteSourceSize.y * scale
    };
    
    Rectangle destRec = {
        drawPos.x,
        drawPos.y,
        frame.frame.width * scale,
        frame.frame.height * scale
    };
    
    Vector2 origin = { 0.0f, 0.0f };
    DrawTexturePro(texture_, frame.frame, destRec, origin, 0.0f, WHITE);
}
