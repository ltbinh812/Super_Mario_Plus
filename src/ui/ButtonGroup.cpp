#include "ButtonGroup.h"
#include "SettingsManager.h"
#include <cmath>
#include <iostream>

float EaseOutBackGroup(float t) {
    const float c1 = 1.70158f;
    const float c3 = c1 + 1.0f;
    return 1.0f + c3 * std::pow(t - 1.0f, 3.0f) + c1 * std::pow(t - 1.0f, 2.0f);
}

ButtonGroup::ButtonGroup() 
    : panelTex_{0}, panelPos_{0,0}, panelScale_(1.0f), btnScale_(1.0f),
      arrowLeft_{0}, arrowRight_{0}, arrowLeftPress_{0}, arrowRightPress_{0},
      arrowAnimTime_(0.0f), hoveredIndex_(-1), currentArrowAlpha_(0.0f), currentArrowOffset_(0.0f),
      titleTex_{0}, titlePos_{0,0}, titleScale_(1.0f), introAnimTime_(1.0f), isEntering_(false),
      groupNameColor_(LIGHTGRAY), groupNameSize_(20.0f), hasHeaderBtn_(false),
      barTex_{0}, barPressTex_{0}, resetBtnTex_{0}, resetBtnPressTex_{0},
      scrollY_(0.0f), maxScrollY_(0.0f), isListeningForKey_(false), listeningRow_(nullptr) {
}

ButtonGroup::~ButtonGroup() {
    for (auto& mbs : buttons_) {
        if (mbs.texNormal.id != 0) UnloadTexture(mbs.texNormal);
        if (mbs.texPress.id != 0) UnloadTexture(mbs.texPress);
    }
    for (auto& mbs : tabs_) {
        if (mbs.texNormal.id != 0) UnloadTexture(mbs.texNormal);
        if (mbs.texPress.id != 0) UnloadTexture(mbs.texPress);
    }
    if (hasHeaderBtn_) {
        if (headerBtn_.texNormal.id != 0) UnloadTexture(headerBtn_.texNormal);
        if (headerBtn_.texPress.id != 0) UnloadTexture(headerBtn_.texPress);
    }
    if (barTex_.id != 0) UnloadTexture(barTex_);
    if (barPressTex_.id != 0) UnloadTexture(barPressTex_);
    if (resetBtnTex_.id != 0) UnloadTexture(resetBtnTex_);
    if (resetBtnPressTex_.id != 0) UnloadTexture(resetBtnPressTex_);
}

void ButtonGroup::LoadSettingsTextures() {
    if (barTex_.id == 0) {
        barTex_ = LoadTexture("assets/UI_screens/bar.png");
        barPressTex_ = LoadTexture("assets/UI_screens/bar_press.png");
        resetBtnTex_ = LoadTexture("assets/UI_screens/bar.png");
        resetBtnPressTex_ = LoadTexture("assets/UI_screens/bar_press.png");
    }
}



void ButtonGroup::SetArrows(Texture2D al, Texture2D ar, Texture2D alp, Texture2D arp) {
    arrowLeft_ = al;
    arrowRight_ = ar;
    arrowLeftPress_ = alp;
    arrowRightPress_ = arp;
}

void ButtonGroup::SetButtonScale(float scale) {
    btnScale_ = scale;
}

void ButtonGroup::SetTitle(Texture2D title, Vector2 pos, float scale) {
    titleTex_ = title;
    titlePos_ = pos;
    titleScale_ = scale;
}

void ButtonGroup::SetGroupName(const std::string& name, Color color, float fontSize) {
    groupName_ = name;
    groupNameColor_ = color;
    groupNameSize_ = fontSize;
}

void ButtonGroup::SetHeaderButton(const std::string& normalPath, const std::string& pressPath, std::function<void()> onClick) {
    headerBtn_.texNormal = LoadTexture(normalPath.c_str());
    headerBtn_.texPress = LoadTexture(pressPath.c_str());
    // Use half btnScale for the header button to make it smaller/fit
    float headerBtnScale = btnScale_ * 0.7f;
    headerBtn_.btn.setSize({(float)headerBtn_.texNormal.width * headerBtnScale, (float)headerBtn_.texNormal.height * headerBtnScale});
    headerBtn_.btn.setOnClick(onClick);
    headerBtn_.isVisible = true; // Always visible when panel is visible
    hasHeaderBtn_ = true;
}

void ButtonGroup::AddDecoLine(Texture2D tex, Vector2 pos, Vector2 scale) {
    decoLines_.push_back({tex, pos, scale});
}

bool ButtonGroup::ContainsPoint(Vector2 point) const {
    if (panelTex_.id == 0) return false;
    
    // Check main panel bounds
    bool inPanel = (point.x >= panelPos_.x && point.x <= panelPos_.x + panelW_ &&
                    point.y >= panelPos_.y && point.y <= panelPos_.y + panelH_);
    if (inPanel) return true;
    
    // Check tabs (they stick out to the left)
    for (const auto& tab : tabs_) {
        if (!tab.isVisible) continue;
        Rectangle bound = { tab.btn.getPosition().x, tab.btn.getPosition().y, 
                            tab.btn.getSize().x, tab.btn.getSize().y };
        if (CheckCollisionPointRec(point, bound)) {
            return true;
        }
    }
    
    return false;
}

void ButtonGroup::AddButton(const std::string& normalPath, const std::string& pressPath, const std::string& action, std::function<void()> onClick, float delay, const std::string& labelText) {
    MenuButtonState mbs;
    mbs.texNormal = LoadTexture(normalPath.c_str());
    mbs.texPress = LoadTexture(pressPath.c_str());
    
    mbs.action = action;
    mbs.labelText = labelText.empty() ? action : labelText;
    
    int fontSize = 12 * panelScale_;
    int textW = MeasureText(mbs.labelText.c_str(), fontSize);
    
    float minW = mbs.texNormal.width * btnScale_;
    float w = std::max(minW, textW + 20.0f * btnScale_); // Add padding
    float h = mbs.texNormal.height * btnScale_;
    
    mbs.btn = Button();
    mbs.btn.setSize({w, h});
    mbs.btn.setOnClick(onClick);
    
    mbs.animationTime = 0.0f;
    mbs.delay = delay;
    mbs.isVisible = false;
    
    buttons_.push_back(mbs);
}

void ButtonGroup::AddTab(const std::string& normalPath, const std::string& pressPath, const std::string& tabName) {
    MenuButtonState mbs;
    mbs.texNormal = LoadTexture(normalPath.c_str());
    mbs.texPress = LoadTexture(pressPath.c_str());
    
    mbs.action = tabName;
    mbs.labelText = tabName;
    
    // Make tabs fixed width so they are equal
    float w = 75.0f * btnScale_;
    float h = mbs.texNormal.height * btnScale_;
    
    mbs.btn = Button();
    mbs.btn.setSize({w, h});
    mbs.btn.setOnClick([this, tabName]() {
        activeTab_ = tabName;
        scrollY_ = 0.0f; // reset scroll when switching tabs
    });
    mbs.animationTime = 1.0f;
    mbs.delay = 0.0f;
    mbs.isVisible = true;
    
    tabs_.push_back(mbs);
}

void ButtonGroup::AddKeybind(const std::string& label, const std::string& actionName, int currentKey, int defaultKey, bool isP1) {
    LoadSettingsTextures();
    KeybindRow row;
    row.label = label;
    row.actionName = actionName;
    row.currentKey = currentKey;
    row.defaultKey = defaultKey;
    row.isP1 = isP1;
    row.isListening = false;
    
    // Key button (uses barTex)
    row.keyBtn.texNormal = barTex_;
    row.keyBtn.texPress = barPressTex_;
    
    // We want key buttons to have a specific width to fit text, but since we don't know the key text until runtime,
    // let's give them a standard larger width initially, e.g. enough for "LongAttack".
    float w1 = 150.0f * btnScale_ / 3.0f; // Approx
    float h1 = row.keyBtn.texNormal.height * btnScale_;
    row.keyBtn.btn = Button();
    row.keyBtn.btn.setSize({w1, h1});
    row.keyBtn.btn.setOnClick([this, actionName, isP1]() {
        // Set listening mode
        isListeningForKey_ = true;
        for(auto& r : keybinds_) {
            if (r.actionName == actionName && r.isP1 == isP1) {
                r.isListening = true;
                listeningRow_ = &r;
            } else {
                r.isListening = false;
            }
        }
    });
    row.keyBtn.isVisible = true;
    
    // Reset button (uses menu_btn_exit)
    row.resetBtn.texNormal = resetBtnTex_;
    row.resetBtn.texPress = resetBtnPressTex_;
    float w2 = row.resetBtn.texNormal.width * btnScale_ * 1.1f; // Wider bar
    float h2 = row.resetBtn.texNormal.height * btnScale_ * 0.8f;
    row.resetBtn.btn = Button();
    row.resetBtn.btn.setSize({w2, h2});
    row.resetBtn.btn.setOnClick([this, actionName, isP1]() {
        // Reset to default
        if (isP1) {
            SettingsManager::GetInstance().ResetP1ToDefault();
            for(auto& r : keybinds_) {
                if (r.isP1) r.currentKey = SettingsManager::GetInstance().GetP1DefaultKey(r.actionName);
            }
        } else {
            SettingsManager::GetInstance().ResetP2ToDefault();
            for(auto& r : keybinds_) {
                if (!r.isP1) r.currentKey = SettingsManager::GetInstance().GetP2DefaultKey(r.actionName);
            }
        }
    });
    row.resetBtn.isVisible = true;
    
    keybinds_.push_back(row);
}

void ButtonGroup::AddSlider(const std::string& label, std::function<float()> getter, std::function<void(float)> setter) {
    LoadSettingsTextures();
    SliderRow row;
    row.label = label;
    row.getter = getter;
    row.setter = setter;
    row.barTex = barTex_;
    row.isDragging = false;
    sliders_.push_back(row);
}


void ButtonGroup::UpdateLayout(float startY, float gap) {
    // Setup Scissor Area for scrolling content
    scissorArea_ = { panelPos_.x + 10.0f * panelScale_, 
                     startY - 10.0f * panelScale_, 
                     panelW_ - 20.0f * panelScale_, 
                     panelH_ - (startY - panelPos_.y) - 30.0f * panelScale_ };
                     
    // Layout regular buttons (Center them properly considering dynamic widths)
    float currentY = startY;
    for (auto& mbs : buttons_) {
        float btnW = mbs.btn.getSize().x;
        mbs.startPos = { panelPos_.x + panelW_ + 100.0f, currentY }; // Slide in from right
        mbs.targetPos = { panelPos_.x + (panelW_ - btnW) / 2.0f, currentY };
        currentY += mbs.btn.getSize().y + gap;
    }
    
    // Layout Tabs (left aligned, cutting through border)
    float tabY = startY + 20.0f * panelScale_;
    float tabGap = gap / 3.0f;
    for (auto& tab : tabs_) {
        float tabW = tab.btn.getSize().x;
        tab.startPos = { panelPos_.x - tabW / 2.0f, tabY };
        tab.targetPos = { panelPos_.x - tabW / 2.0f, tabY };
        tab.btn.setPosition(tab.targetPos);
        // Expand hitbox slightly on all sides to make it easier to click without overlapping other UI
        tab.btn.expandHitbox(15.0f * panelScale_, 15.0f * panelScale_, 10.0f * panelScale_, 10.0f * panelScale_);
        tabY += tab.btn.getSize().y + tabGap;
    }
    
    // Layout Keybind Rows (placed to the right of Tabs)
    float rowY = startY;
    float rowGap = gap * 0.8f;
    for (auto& row : keybinds_) {
        // Label pos implicitly derived in Render based on rowY
        
        // Key button pos
        Vector2 keyPos = { panelPos_.x + 180.0f * panelScale_, rowY }; // shift right for label
        row.keyBtn.startPos = keyPos;
        row.keyBtn.targetPos = keyPos;
        row.keyBtn.btn.setPosition(keyPos);
        
        // Reset button pos
        Vector2 resetPos = { panelPos_.x + 250.0f * panelScale_, rowY };
        row.resetBtn.startPos = resetPos;
        row.resetBtn.targetPos = resetPos;
        row.resetBtn.btn.setPosition(resetPos);
        
        rowY += row.keyBtn.texNormal.height * btnScale_ + rowGap;
    }
    
    // Layout Sliders
    float sliderY = startY;
    for (auto& row : sliders_) {
        float rowHeight = row.barTex.height * btnScale_;
        float barW = row.barTex.width * btnScale_ * 2.5f; // Make bar longer
        float barH = rowHeight * 0.4f; // Make bar thinner vertically
        
        // Center the bar vertically within the standard row height
        float barY = sliderY + (rowHeight - barH) / 2.0f;
        Vector2 barPos = { panelPos_.x + 180.0f * panelScale_, barY };
        
        row.barRect = { barPos.x, barPos.y, barW, barH };
        row.knobRect.width = 12.0f * panelScale_; // Make knob even smaller
        row.knobRect.height = 12.0f * panelScale_;
        
        sliderY += rowHeight + rowGap;
    }
    
    float maxY = std::max(rowY, sliderY);
    maxScrollY_ = std::max(0.0f, (maxY - startY) - scissorArea_.height + 20.0f * panelScale_);

    
    if (hasHeaderBtn_) {
        float headerY = panelPos_.y + 20.0f * panelScale_;
        float undoX = panelPos_.x + 15.0f * panelScale_;
        headerBtn_.btn.setPosition({undoX, headerY});
    }
}

void ButtonGroup::TriggerEntry() {
    isEntering_ = true;
    introAnimTime_ = 0.0f;
    for (auto& mbs : buttons_) {
        mbs.animationTime = 0.0f;
        mbs.isVisible = false;
        mbs.btn.setPosition(mbs.startPos);
        mbs.btn.updateBound();
    }
}

void ButtonGroup::Update(float dt) {
    if (isEntering_) {
        introAnimTime_ += dt * 1.5f;
        if (introAnimTime_ >= 1.0f) {
            introAnimTime_ = 1.0f;
            isEntering_ = false;
        }
    }
    
    float yOffset = 0.0f;
    if (introAnimTime_ < 1.0f) {
        float t = EaseOutBackGroup(introAnimTime_);
        yOffset = -800.0f * (1.0f - t);
    }
    
    if (hasHeaderBtn_) {
        // Base scale to match title height
        float targetHeight = groupNameSize_ * panelScale_ * 1.3f;
        float baseScale = targetHeight / (float)headerBtn_.texNormal.height;
        
        float headerXOffset = 15.0f * panelScale_;
        float headerYOffset = 15.0f * panelScale_;
        Vector2 btnPos = { panelPos_.x + headerXOffset, panelPos_.y + yOffset + headerYOffset };
        
        // Hitbox always matches the base scale exactly
        headerBtn_.btn.setSize({(float)headerBtn_.texNormal.width * baseScale, (float)headerBtn_.texNormal.height * baseScale});
        headerBtn_.btn.setPosition(btnPos);
        headerBtn_.btn.updateBound();
        headerBtn_.btn.update();
        
        // Visual scale updates dynamically
        if (headerBtn_.btn.isPressed()) {
            headerBtnCurrentScale_ = baseScale * 0.7f; // shrink
        } else if (headerBtn_.btn.isHovered()) {
            headerBtnCurrentScale_ = baseScale * 1.55f; // enlarge
        } else {
            headerBtnCurrentScale_ = baseScale;
        }
    }

    bool anyHovered = false;
    for (size_t i = 0; i < buttons_.size(); ++i) {
        auto& mbs = buttons_[i];
        
        // Slide in animation
        if (mbs.delay > 0.0f) {
            mbs.delay -= dt;
            if (mbs.delay <= 0.0f) {
                mbs.isVisible = true;
            }
        }
        
        if (mbs.isVisible && mbs.animationTime < 1.0f) {
            mbs.animationTime += dt * 1.5f;
            if (mbs.animationTime > 1.0f) mbs.animationTime = 1.0f;
            float t = EaseOutBackGroup(mbs.animationTime);
            Vector2 currentPos = {
                mbs.startPos.x + (mbs.targetPos.x - mbs.startPos.x) * t,
                mbs.startPos.y + (mbs.targetPos.y - mbs.startPos.y) * t
            };
            mbs.btn.setPosition(currentPos);
            mbs.btn.updateBound();
        }
        
        mbs.btn.update();
        
        if (mbs.btn.isHovered()) {
            anyHovered = true;
            hoveredIndex_ = (int)i;
        }
    }
    
    // Check clicks on Keybind Rows (within scissor area)
    for (auto& row : keybinds_) {
        // Only interact if visible
        float rowY = row.keyBtn.btn.getPosition().y - scrollY_;
        if (rowY + row.keyBtn.btn.getSize().y < scissorArea_.y || rowY > scissorArea_.y + scissorArea_.height) {
            continue;
        }
        
        row.keyBtn.btn.update();
        
        // Only allow reset if not already default
        if (row.currentKey != row.defaultKey) {
            row.resetBtn.btn.update();
        }
    }
    
    // Check clicks on Tabs
    for (auto& tab : tabs_) {
        if (tab.labelText != activeTab_) {
            tab.btn.update();
        }
    }
    
    if (!anyHovered) {
        hoveredIndex_ = -1;
    }
    
    // Arrow animation
    if (hoveredIndex_ != -1) {
        currentArrowAlpha_ += dt * 1500.0f; // fast fade in
        if (currentArrowAlpha_ > 255.0f) currentArrowAlpha_ = 255.0f;
        
        currentArrowOffset_ += dt * 50.0f; // slide in slightly
        if (currentArrowOffset_ > 10.0f) currentArrowOffset_ = 10.0f;
    } else {
        currentArrowAlpha_ -= dt * 1000.0f; // fade out
        if (currentArrowAlpha_ < 0.0f) {
            currentArrowAlpha_ = 0.0f;
            currentArrowOffset_ = 0.0f;
        }
    }
}

void ButtonGroup::HandleInput(Vector2 mousePos, bool mousePressed, bool mouseReleased) {
    if (isListeningForKey_ && listeningRow_ != nullptr) {
        int key = GetKeyPressed();
        if (key > 0) { // Some key was pressed
            if (listeningRow_->isP1) {
                SettingsManager::GetInstance().SetP1Key(listeningRow_->actionName, key);
            } else {
                SettingsManager::GetInstance().SetP2Key(listeningRow_->actionName, key);
            }
            listeningRow_->currentKey = key;
            listeningRow_->isListening = false;
            isListeningForKey_ = false;
            listeningRow_ = nullptr;
        }
        return; // Block other inputs
    }

    if (!buttons_.empty() || !keybinds_.empty()) {
        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {
            scrollY_ -= wheel * 20.0f; // Scroll speed
            if (scrollY_ < 0.0f) scrollY_ = 0.0f;
            if (scrollY_ > maxScrollY_) scrollY_ = maxScrollY_;
        }
    }

    // Offset mouse pos for scrolled items
    Vector2 scrolledMouse = { mousePos.x, mousePos.y + scrollY_ };

    for (auto& mbs : buttons_) {
        if (!mbs.isVisible) continue;
        if (CheckCollisionPointRec(scrolledMouse, scissorArea_)) {
            mbs.btn.handleInput(scrolledMouse, mousePressed, mouseReleased);
        }
    }
    
    for (auto& tab : tabs_) {
        if (!tab.isVisible) continue;
        tab.btn.handleInput(mousePos, mousePressed, mouseReleased); // Tabs don't scroll
    }
    
    if (activeTab_ == "Controls") {
        for (auto& row : keybinds_) {
            if (CheckCollisionPointRec(mousePos, scissorArea_)) {
                row.keyBtn.btn.handleInput(scrolledMouse, mousePressed, mouseReleased);
                if (row.currentKey != row.defaultKey)
                    row.resetBtn.btn.handleInput(scrolledMouse, mousePressed, mouseReleased);
            }
        }
    } else if (activeTab_ == "Sounds") {
        for (auto& row : sliders_) {
            if (CheckCollisionPointRec(mousePos, scissorArea_)) {
                Rectangle hitBox = row.barRect;
                // Hitbox covers the bar and the larger knob
                float knobOverflowY = (row.knobRect.height - hitBox.height) / 2.0f;
                hitBox.y -= std::max(10.0f * panelScale_, knobOverflowY);
                hitBox.height += std::max(20.0f * panelScale_, knobOverflowY * 2.0f);
                
                // Add some padding to width to allow dragging the knob at edges
                hitBox.x -= row.knobRect.width / 2.0f;
                hitBox.width += row.knobRect.width;
                
                if (mousePressed && CheckCollisionPointRec(scrolledMouse, hitBox)) {
                    row.isDragging = true;
                }
                
                if (row.isDragging) {
                    float localX = scrolledMouse.x - row.barRect.x;
                    float percent = localX / row.barRect.width;
                    row.setter(percent);
                }
            }
            if (mouseReleased) {
                row.isDragging = false;
            }
        }
    }


    if (hasHeaderBtn_) {
        headerBtn_.btn.handleInput(mousePos, mousePressed, mouseReleased);
    }
}

void ButtonGroup::Render() const {
    if (panelTex_.id == 0) return;
    
    float yOffset = isEntering_ ? (1.0f - EaseOutBackGroup(introAnimTime_)) * -800.0f : 0.0f;
    
    // Draw Panel Background (Using DrawTexturePro to stretch evenly)
    if (panelTex_.id != 0) {
        Rectangle source = {0.0f, 0.0f, (float)panelTex_.width, (float)panelTex_.height};
        Rectangle dest = {panelPos_.x, panelPos_.y + yOffset, panelW_, panelH_};
        Vector2 origin = {0.0f, 0.0f};
        DrawTexturePro(panelTex_, source, dest, origin, 0.0f, WHITE);
    }
    
    // Draw Title if exists
    if (titleTex_.id != 0) {
        DrawTextureEx(titleTex_, {titlePos_.x, titlePos_.y + yOffset}, 0.0f, titleScale_, WHITE);
    }
    
    // Draw Deco Lines
    for (const auto& line : decoLines_) {
        Rectangle source = { 0.0f, 0.0f, (float)line.tex.width, (float)line.tex.height };
        Rectangle dest = { line.pos.x, line.pos.y + yOffset, line.tex.width * line.scale.x, line.tex.height * line.scale.y };
        DrawTexturePro(line.tex, source, dest, {0,0}, 0.0f, WHITE);
    }
    
    float headerXOffset = 0.0f;
    float headerYOffset = 0.0f;
    
    // Calculate Header Base Positions
    float headerY = panelPos_.y + 20.0f * panelScale_;
    
    // Draw Header Button
    if (hasHeaderBtn_) {
        Vector2 btnPos = headerBtn_.btn.getPosition();
        
        // Recalculate baseScale to compute center-expansion difference
        float targetHeight = groupNameSize_ * panelScale_ * 1.3f;
        float baseScale = targetHeight / (float)headerBtn_.texNormal.height;
        
        float diffX = (headerBtn_.texNormal.width * baseScale - headerBtn_.texNormal.width * headerBtnCurrentScale_) / 2.0f;
        float diffY = (headerBtn_.texNormal.height * baseScale - headerBtn_.texNormal.height * headerBtnCurrentScale_) / 2.0f;
        
        Texture2D texToDraw = headerBtn_.btn.isPressed() ? headerBtn_.texPress : headerBtn_.texNormal;
        DrawTextureEx(texToDraw, {btnPos.x + diffX, btnPos.y + diffY + yOffset}, 0.0f, headerBtnCurrentScale_, WHITE);
        
        headerXOffset += headerBtn_.texNormal.width * baseScale + 10.0f * panelScale_;
    }
    
    // Draw Group Name (Centered)
    if (!groupName_.empty()) {
        float textYOffset = 0.0f;
        if (hasHeaderBtn_) {
            float targetHeight = groupNameSize_ * panelScale_ * 1.3f;
            textYOffset = (targetHeight - (groupNameSize_ * panelScale_)) / 2.0f;
        }
        
        int fSize = groupNameSize_ * panelScale_;
        int textW = MeasureText(groupName_.c_str(), fSize);
        float titleX = panelPos_.x + (panelW_ - textW) / 2.0f;
        
        DrawText(groupName_.c_str(), (int)titleX, (int)(headerY + yOffset + headerYOffset + textYOffset), fSize, groupNameColor_);
    }
    
    // Draw Tabs (Outside Scissor)
    for (const auto& tab : tabs_) {
        if (!tab.isVisible) continue;
        Vector2 pos = tab.btn.getPosition();
        Texture2D texToDraw = (tab.btn.isPressed() || tab.labelText == activeTab_) ? tab.texPress : tab.texNormal;
        float btnW = tab.btn.getSize().x;
        float btnH = tab.btn.getSize().y;
        
        NPatchInfo nPatch = { {0, 0, (float)texToDraw.width, (float)texToDraw.height}, 6, 6, 6, 6, NPATCH_NINE_PATCH };
        DrawTextureNPatch(texToDraw, nPatch, {pos.x, pos.y + yOffset, btnW, btnH}, {0,0}, 0.0f, WHITE);
        
        // Draw Text inside Tab
        int fontSize = 12 * panelScale_;
        int textW = MeasureText(tab.labelText.c_str(), fontSize);
        DrawText(tab.labelText.c_str(), pos.x + (btnW - textW)/2, pos.y + yOffset + (btnH - fontSize)/2, fontSize, BLACK);
    }
    
    // Render scrollable content with scissor
    BeginScissorMode(scissorArea_.x, scissorArea_.y + yOffset, scissorArea_.width, scissorArea_.height);
    
    // Draw Regular Buttons
    for (const auto& mbs : buttons_) {
        if (!mbs.isVisible) continue;
        
        Vector2 pos = mbs.btn.getPosition();
        Texture2D texToDraw = mbs.btn.isPressed() ? mbs.texPress : mbs.texNormal;
        float btnW = mbs.btn.getSize().x;
        float btnH = mbs.btn.getSize().y;
        
        NPatchInfo nPatch = { {0, 0, (float)texToDraw.width, (float)texToDraw.height}, 6, 6, 6, 6, NPATCH_NINE_PATCH };
        DrawTextureNPatch(texToDraw, nPatch, {pos.x, pos.y - scrollY_ + yOffset, btnW, btnH}, {0,0}, 0.0f, WHITE);
        
        // Draw Text
        int fontSize = 12 * panelScale_;
        int textW = MeasureText(mbs.labelText.c_str(), fontSize);
        DrawText(mbs.labelText.c_str(), pos.x + (btnW - textW)/2, pos.y - scrollY_ + yOffset + (btnH - fontSize)/2, fontSize, BLACK);
    }

    if (activeTab_ == "Controls") {
        for (const auto& row : keybinds_) {
            Vector2 kPos = row.keyBtn.btn.getPosition();
            Vector2 rPos = row.resetBtn.btn.getPosition();
            
            // Draw Label
            int fontSize = 12 * panelScale_;
            DrawText(row.label.c_str(), panelPos_.x + 55.0f * panelScale_, kPos.y - scrollY_ + yOffset + (row.keyBtn.btn.getSize().y - fontSize)/2, fontSize, BLACK);
            
            // Draw Key Button
            Texture2D kTex = row.keyBtn.btn.isPressed() ? row.keyBtn.texPress : row.keyBtn.texNormal;
            DrawTextureEx(kTex, {kPos.x, kPos.y - scrollY_ + yOffset}, 0.0f, btnScale_, WHITE);
            
            std::string keyText = row.isListening ? "_" : SettingsManager::GetInstance().GetKeyName(row.currentKey);
            int kTextW = MeasureText(keyText.c_str(), fontSize);
            float kw = kTex.width * btnScale_;
            float kh = kTex.height * btnScale_;
            DrawText(keyText.c_str(), kPos.x + (kw - kTextW)/2, kPos.y - scrollY_ + yOffset + (kh - fontSize)/2, fontSize, BLACK);
            
            // Draw arrows if listening
            if (row.isListening && arrowLeft_.id != 0 && arrowRight_.id != 0) {
                float arrowScale = btnScale_ * 0.5f;
                float alW = arrowLeft_.width * arrowScale;
                float alH = arrowLeft_.height * arrowScale;
                float alX = kPos.x - alW - 5.0f * panelScale_;
                float alY = kPos.y - scrollY_ + yOffset + (kh - alH) / 2.0f;
                DrawTextureEx(arrowLeft_, {alX, alY}, 0.0f, arrowScale, WHITE);
                
                float arW = arrowRight_.width * arrowScale;
                float arH = arrowRight_.height * arrowScale;
                float arX = kPos.x + kw + 5.0f * panelScale_;
                float arY = kPos.y - scrollY_ + yOffset + (kh - arH) / 2.0f;
                DrawTextureEx(arrowRight_, {arX, arY}, 0.0f, arrowScale, WHITE);
            }
            
            // Draw Reset Button
            bool isDefault = (row.currentKey == row.defaultKey);
            Texture2D rTex = (row.resetBtn.btn.isPressed() || isDefault) ? row.resetBtn.texPress : row.resetBtn.texNormal;
            
            float rw = rTex.width * btnScale_ * 1.1f; // Wider bar
            float rh = rTex.height * btnScale_ * 0.8f;
            Rectangle source = {0.0f, 0.0f, (float)rTex.width, (float)rTex.height};
            Rectangle dest = {rPos.x, rPos.y - scrollY_ + yOffset, rw, rh};
            DrawTexturePro(rTex, source, dest, {0,0}, 0.0f, WHITE);
            
            std::string btnText = isDefault ? "Default" : "Reset";
            int rTextW = MeasureText(btnText.c_str(), fontSize * 0.8f);
            DrawText(btnText.c_str(), rPos.x + (rw - rTextW)/2, rPos.y - scrollY_ + yOffset + (rh - fontSize*0.8f)/2, fontSize * 0.8f, BLACK);
        }
    } else if (activeTab_ == "Sounds") {
        for (const auto& row : sliders_) {
            float sY = row.barRect.y - scrollY_ + yOffset;
            float barCenterY = sY + row.barRect.height / 2.0f;
            
            // Draw Label
            int fontSize = 12 * panelScale_;
            DrawText(row.label.c_str(), panelPos_.x + 55.0f * panelScale_, barCenterY - fontSize/2.0f, fontSize, BLACK);
            
            // Draw Bar Background
            Rectangle dest = { row.barRect.x, sY, row.barRect.width, row.barRect.height };
            Rectangle source = {0.0f, 0.0f, (float)row.barTex.width, (float)row.barTex.height};
            DrawTexturePro(row.barTex, source, dest, {0,0}, 0.0f, WHITE);
            
            // Draw Bar Fill
            float val = row.getter();
            Rectangle fillDest = dest;
            fillDest.width = dest.width * val;
            Rectangle fillSource = source;
            fillSource.width = source.width * val;
            DrawTexturePro(row.barTex, fillSource, fillDest, {0,0}, 0.0f, GREEN); // Highlighted fill
            
            // Draw Knob
            float knobSize = 12.0f * panelScale_; // Match layout size
            float knobX = dest.x + val * dest.width - knobSize / 2.0f;
            float knobY = dest.y + dest.height / 2.0f - knobSize / 2.0f;
            Rectangle knobRec = { knobX, knobY, knobSize, knobSize };
            
            // Draw Shadow
            DrawRectangleRounded({knobX + 1, knobY + 1, knobSize, knobSize}, 0.3f, 8, {0, 0, 0, 100});
            
            // Draw Knob Base
            Color knobColor = row.isDragging ? ORANGE : RAYWHITE;
            DrawRectangleRounded(knobRec, 0.3f, 8, knobColor);
            DrawRectangleRoundedLines(knobRec, 0.3f, 8, 1.5f * panelScale_, DARKGRAY);
            
            // Draw Percentage
            std::string perc = std::to_string((int)(val * 100)) + "%";
            DrawText(perc.c_str(), dest.x + dest.width + 15.0f * panelScale_, barCenterY - fontSize/2.0f, fontSize, BLACK);
        }
    }

    
    EndScissorMode();
}
