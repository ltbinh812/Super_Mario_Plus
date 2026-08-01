# Punch Combo System (punch1 → punch2 → punch3 → punch4)

## Overview

Implement a combo chain for punches. When the player presses the attack button while already performing a punch, the **next punch** in the chain is queued and executes when the current one finishes. The chain is: `punch1 → punch2 → punch3 → punch4`. Non-combo skills (like Dash) always start from `punch1`.

## Design: "Next Skill" Queuing in PlayerSkillState

Your proposed approach is a good one. Here's how it works:

```
┌──────────────┐  onAttack()  ┌──────────────┐  timer ends  ┌──────────────┐
│  punch1      │ ──────────►  │  punch1      │ ──────────►  │  punch2      │
│  playing     │              │  nextSkill   │              │  playing     │
│  nextSkill=∅ │              │  = punch2    │              │  nextSkill=∅ │
└──────────────┘              └──────────────┘              └──────────────┘
```

- While in `PlayerSkillState` performing a **combo-capable skill**, pressing attack queues the next skill in the chain via `nextSkill`.
- When the current skill's timer expires, if `nextSkill` exists → transition into it (re-enter `skillState` with the queued skill). Otherwise → return to idle.
- Non-combo skills (e.g., Dash) don't set a next skill.

## Proposed Changes

### ISkill — Add combo chain support

#### [MODIFY] [ISkill.h](file:///d:/super_mario/Super_Mario_Plus/include/entity/Skill/ISkill.h)

Add a `getNextComboSkillName()` virtual method that returns the name of the next skill in the combo chain (or `""` if none). This keeps combo knowledge inside each skill.

```diff
 class ISkill {
 protected:
     float manaCost;
     float duration;
     std::string animationName;
+    std::string nextComboSkillName = "";  // Name of the next skill in the combo chain
 public: 
     ISkill(float mn, float dr) : manaCost(mn), duration(dr) {}
     virtual ~ISkill() = default;
     virtual void execute(Player& player) = 0;
 
     float getManaCost() const { return manaCost; }
     float getDuration() const { return duration; }
     const std::string& getAnimationName() const { return animationName; }
+    const std::string& getNextComboSkillName() const { return nextComboSkillName; }
+    bool hasNextCombo() const { return !nextComboSkillName.empty(); }
 };
```

---

### New Punch Skills — punch2, punch3, punch4

#### [MODIFY] [Punch1Skill.h](file:///d:/super_mario/Super_Mario_Plus/include/entity/Skill/Punch1Skill.h)

Set `nextComboSkillName = "Punch2"` in the constructor so punch1 chains into punch2.

```diff
 class Punch1Skill : public ISkill {
 public:
     Punch1Skill(float mn, float dr) : ISkill(mn, dr) {
         animationName = "punch1";
+        nextComboSkillName = "Punch2";
     }
     void execute(Player& player) override;
 };
```

#### [NEW] [Punch2Skill.h](file:///d:/super_mario/Super_Mario_Plus/include/entity/Skill/Punch2Skill.h)

Same structure as Punch1Skill, with `animationName = "punch2"`, `nextComboSkillName = "Punch3"`.

#### [NEW] [Punch3Skill.h](file:///d:/super_mario/Super_Mario_Plus/include/entity/Skill/Punch3Skill.h)

Same structure, `animationName = "punch3"`, `nextComboSkillName = "Punch4"`.

#### [NEW] [Punch4Skill.h](file:///d:/super_mario/Super_Mario_Plus/include/entity/Skill/Punch4Skill.h)

Same structure, `animationName = "punch4"`, `nextComboSkillName = ""` (end of chain).

#### [NEW] [Punch2Skill.cpp](file:///d:/super_mario/Super_Mario_Plus/src/entity/Skill/Punch2Skill.cpp) / [Punch3Skill.cpp](file:///d:/super_mario/Super_Mario_Plus/src/entity/Skill/Punch3Skill.cpp) / [Punch4Skill.cpp](file:///d:/super_mario/Super_Mario_Plus/src/entity/Skill/Punch4Skill.cpp)

Empty `execute()` bodies (same as current Punch1Skill), to be filled later with gameplay effects.

---

### PlayerSkillState — Combo queuing logic

#### [MODIFY] [PlayerSkillState.h](file:///d:/super_mario/Super_Mario_Plus/include/entity/Player/PlayerSkillState.h)

Add `nextSkill` pointer to queue the next skill in the chain.

```diff
 class PlayerSkillState : public PlayerState {
     private:
     ISkill* currentSkill = nullptr;
+    ISkill* nextSkill = nullptr;  // Queued combo skill
     float timer = 0.0f;
     ...
 };
```

#### [MODIFY] [PlayerSkillState.cpp](file:///d:/super_mario/Super_Mario_Plus/src/entity/Player/PlayerSkillState.cpp)

**`onAttack()`** — When the player presses attack during a skill, look up the next combo skill name from `currentSkill` and queue it:

```cpp
void PlayerSkillState::onAttack() {
    if (!currentSkill || !currentSkill->hasNextCombo()) return;
    
    // Look up the next combo skill from the player's skill list
    ISkill* combo = player.findSkill(currentSkill->getNextComboSkillName());
    if (combo && player.hasEnoughMana(combo->getManaCost())) {
        nextSkill = combo;
    }
}
```

**`update()`** — When the timer expires, check `nextSkill`:

```diff
 void PlayerSkillState::update(float dt) {
     timer -= dt;
     timer = std::max(timer, 0.0f);
 
     if (timer == 0) {
-        player.requestState(player.idleState);
+        if (nextSkill) {
+            // Chain into the next combo skill
+            ISkill* next = nextSkill;
+            nextSkill = nullptr;
+            currentSkill = next;
+            onEnter();  // Re-enter with the new skill
+        } else {
+            player.requestState(player.idleState);
+        }
     }
 }
```

**`onExit()`** — Clear both pointers:

```diff
 void PlayerSkillState::onExit() {
     currentSkill = nullptr;
+    nextSkill = nullptr;
     timer = 0.0f;
 }
```

---

### Player — Add helper methods

#### [MODIFY] [Player.h](file:///d:/super_mario/Super_Mario_Plus/include/entity/Player/Player.h)

```diff
+  ISkill* findSkill(const std::string& skillName);
+  bool hasEnoughMana(float cost) const;
```

#### [MODIFY] [Player.cpp](file:///d:/super_mario/Super_Mario_Plus/src/entity/Player/Player.cpp)

```cpp
ISkill* Player::findSkill(const std::string& skillName) {
    auto it = skillList.find(skillName);
    if (it != skillList.end()) return it->second.get();
    return nullptr;
}

bool Player::hasEnoughMana(float cost) const {
    return runtimeStats.mana >= static_cast<int>(cost);
}
```

---

### PlayerFactory — Register new punch skills

#### [MODIFY] [PlayerFactory.cpp](file:///d:/super_mario/Super_Mario_Plus/src/entity/Player/PlayerFactory.cpp)

- Add `#include` for Punch2Skill, Punch3Skill, Punch4Skill
- Register them in the skill list
- Add `addAnimation()` calls for "punch2", "punch3", "punch4"

---

### characters.json — Add animation/skill data

#### [MODIFY] [characters.json](file:///d:/super_mario/Super_Mario_Plus/assets/config/characters.json)

Add `punch2`, `punch3`, `punch4` animation and skill entries (following the existing `punch1` pattern).

> [!IMPORTANT]
> You'll need sprite sheets for punch2, punch3, and punch4 animations. Are these assets already available, or should I use placeholder entries pointing to the existing punch1 texture for now?

---

### CMakeLists.txt — Add new source files

#### [MODIFY] [CMakeLists.txt](file:///d:/super_mario/Super_Mario_Plus/CMakeLists.txt)

Add the new `.cpp` files to the SOURCES list:

```diff
     src/entity/Skill/PunchSkill.cpp
+    src/entity/Skill/Punch2Skill.cpp
+    src/entity/Skill/Punch3Skill.cpp
+    src/entity/Skill/Punch4Skill.cpp
```

## Open Questions

> [!IMPORTANT]
> **Sprite assets**: Do you already have `punch2`, `punch3`, `punch4` sprite sheets and texture entries in your asset manager? If not, should I use the `punch1` texture as a placeholder?

> [!IMPORTANT]
> **Mana cost per combo hit**: Should each punch in the chain cost mana individually (deducted when the combo skill starts), or should only the initial `punch1` cost mana?

> [!NOTE]
> **Existing inconsistency**: The CMakeLists.txt references `src/entity/Skill/PunchSkill.cpp` but the actual file is `Punch1Skill.cpp`, and PlayerFactory includes `"PunchSkill.h"` but the header is `Punch1Skill.h`. This seems like a rename mismatch. Should I fix this naming inconsistency while implementing the combo system?

## Verification Plan

### Build Verification
- Run `cmake --build` to ensure all new files compile and link correctly.

### Manual Verification
- Press attack (KEY_J) to trigger punch1. Press attack again during punch1 → punch2 should queue and play after punch1 finishes.
- Continue pressing attack to chain through punch3 → punch4.
- After punch4, pressing attack should not queue anything (end of chain → return to idle).
- Using Dash (KEY_L) during idle/run should still work normally (no combo chain).
- Verify mana deduction happens at the right time.
