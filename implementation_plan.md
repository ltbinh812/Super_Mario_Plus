# Detailed Refactoring Plan: BaseLevelState

Since you want to keep `HandleInput` as is (which is completely fine for readability), we will focus our refactoring on the remaining functions. Here is the **exhaustive list** of every single thing that needs to be refactored in each function of `BaseLevelState.cpp`.

---

## 1. `BaseLevelState::BaseLevelState()` (Constructor)
*   **Hardcoded Input Bindings:** You have `player1Handler.bindKey(KEY_A, ...)` hardcoded for dozens of keys. 
    *   *Refactor:* This violates the OCP and makes it impossible to implement a "Keybindings Settings" menu. Move this into an `InputManager` or read bindings from a JSON file.
*   **Entity/Item Spawning by String Matching:** You distinguish mobs from items using `data.identifier.rfind("Mob_", 0) == 0`. 
    *   *Refactor:* This is extremely fragile (a typo in LDtk breaks the game). Use LDtk's Custom Fields or Tags (e.g., `data.type == "Enemy"`) to determine the factory to use.

## 2. `BaseLevelState::HandleInput()`
*   *As requested, we will keep this function mostly as is.*
*   *Minor note for the future:* Calling `ent->decideAction()` (AI thinking) here is technically "Processing" rather than reading player hardware input, but it's safe to leave it here if it just queues commands.

## 3. `BaseLevelState::Process()`
*   **Encapsulation Breach (Respawn):** When `respawnTimer <= 0`, you manually do: `player1->getRuntimeStatsMutable().health = maxHealth`, set `velocity = {0,0}`, and call `player1->forceState(idleState)`. 
    *   *Refactor:* LevelState should not micromanage the player. Create `Player::respawn(Vector2 pos)` and just call that.
*   **Encapsulation Breach (Death Check):** The lambda `checkDeathCondition` directly reads `health` and `position.y`.
    *   *Refactor:* Add `player->isDead()` and `player->isOutOfBounds(map.GetHeight())`.
*   **Manual Kill Execution:** You call `takeDamage(9999)` if they fall out of bounds.
    *   *Refactor:* Add a `Player::kill()` or `Player::instantDie()` method.
*   **The Explosion System:** You peek `ExplosionDamage` from the queue, manually check `CheckCollisionRecs`, and hardcode `std::make_unique<LavaEffect>()`.
    *   *Refactor:* The `ExplosionDamage` command should contain an `EffectType` (or an `EffectSpawner` functor). This allows ice bombs or poison bombs without ever modifying `BaseLevelState`.
*   **MISSING LOGIC (To be moved here from Update):**
    *   *Refactor:* We MUST move Player-to-Player Pushing, Map Edge Transitions, Item Interactions, and Spawning from `Update()` into `Process()`.

## 4. `BaseLevelState::Update(float dt)`
*   **Player-to-Player Pushing:** A massive block of collision resolution and wall-checking logic is currently sitting in `Update()`.
    *   *Refactor:* Move this ENTIRE block to `Process()`.
*   **Map Edge Transition:** The logic checking `pX > map.GetWidth() - edge` to trigger level loading.
    *   *Refactor:* Move to `Process()`.
*   **OCP Violation: Coin Magnet Logic:** You use `dynamic_cast<Coin*>(item.get())` and manually calculate vectors to pull the coin to the player.
    *   *Refactor:* Delete this from `BaseLevelState`. Move the magnet logic inside `Coin::process(const std::vector<Player*>& players)`. The coin should manage its own physics!
*   **OCP Violation: Flag Save Logic:** You use `dynamic_cast<Flag*>(item.get())` to check if a flag was touched, then trigger `SaveManager`.
    *   *Refactor:* Delete this. Put the save logic inside `Flag::onInteract(Player& p)`.
*   **Item Interaction Loop:** The loop that checks `CheckCollisionRecs(itemBox, playerBox)` and calls `item->onInteract()`.
    *   *Refactor:* Move to `Process()`.
*   **Entity/Item Spawning:** Consuming `SpawnCategory::Entity` and `SpawnCategory::Item` from the queue.
    *   *Refactor:* Move to `Process()`.
*   *(After refactoring, `Update()` will be beautifully short: just calculating `dynamicSolids` and calling `updatePhysicsWithMap` and `update(dt)` for players, items, and entities).*

## 5. `BaseLevelState::Render(float alpha)`
*   **Array Reconstruction:** You manually recreate `std::vector<Entity*> allEntities` to pass into `combatSystem.renderDebug()`. 
    *   *Refactor:* Minor issue, but we could maintain a single cached array of active entity pointers in `Process()` to avoid rebuilding it every frame in `Render()`.

## 6. `BaseLevelState::TransitionToLevel(...)`
*   **Code Duplication (Spawning):** You completely copy-pasted the LDtk entity spawning logic (`rfind("Mob_")`, checking `persistedItemStates`, etc.) from the Constructor into this function.
    *   *Refactor:* Extract this massive loop into a helper private method: `void SpawnLevelEntities()`. Call this method in the constructor and here.

## 7. `BaseLevelState::createSaveData()` & `restoreFromSaveData(...)`
*   **Encapsulation Breach (Saving):** You manually rip out `pd.health`, `pd.mana`, `pd.posX`, `pd.storedItemSlot` from the Player to save them, and inject them back to load them.
    *   *Refactor:* The Player should know how to save itself. Create `PlayerSaveData Player::createSaveData()` and `void Player::restoreFromSaveData(const PlayerSaveData& data)`. `BaseLevelState` should just delegate to the Player.
*   **Code Duplication (Spawning AGAIN):** You copy-pasted the LDtk spawning loop a **third time** in `restoreFromSaveData`.
    *   *Refactor:* Call the newly created `SpawnLevelEntities()` helper method.

---

### Final Question

This is the absolute maximal breakdown of everything wrong in the class. 

Do you want me to execute all of these changes at once, or do you want to break it down and have me execute **just the Game Loop & Encapsulation fixes (Process & Update)** first?
