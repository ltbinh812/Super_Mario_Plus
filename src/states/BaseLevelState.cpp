#include "BaseLevelState.h"
#include "MainMenuState.h"
#include "StateCommands.h"
#include "core/SettingsManager.h"
#include "Effects.h"
#include "EntityFactory.h"
#include "GameState.h"
#include "ItemAtlasRegistry.h"
#include "ItemFactory.h"
#include "PlayerCommands.h"
#include "PlayerFactory.h"
#include "SaveManager.h"
#include "Flag.h"
#include "PlayerHUD.h"
#include "Coin.h"
#include "CustomMapData.h"
#include <algorithm>
#include <iostream>
#include <raylib.h>
#include <raymath.h>


BaseLevelState::BaseLevelState(const std::string &mapFilePath,
                               const std::string &initialLevel,
                               const std::string &p1Name,
                               const std::string &p2Name)
    : mapCamera(416.0f), currentLevel(initialLevel), mapFilePath(mapFilePath) {

  bool loaded = map.LoadLDtkMap(mapFilePath, currentLevel);
  if (loaded && map.GetHeight() > 0) {
    if (currentLevel.empty())
      currentLevel = map.GetCurrentLevelName();
    std::cout << "[BaseLevelState] Loaded " << mapFilePath << " ("
              << currentLevel << ") successfully!\n";

    auto spawns = map.GetPlayerSpawns();
    Vector2 spawn1 = spawns.size() > 0 ? spawns[0] : Vector2{180.0f, 208.0f};

    partyInventory = std::make_shared<PartyInventory>();

    player1 = PlayerFactory::createPlayer(p1Name, {0, 0});
    if (player1) {
      player1->setPosition(spawn1);
      player1->setStartPosition(spawn1);
      player1->setCommandQueue(&spawnQueue);
      player1->setPartyInventory(partyInventory);
      
      auto& sm = SettingsManager::GetInstance();
      player1Handler.bindKey(sm.GetP1Key("Move Left"), std::make_unique<MoveLeftCommand>(), InputType::DOWN);
      player1Handler.bindKey(sm.GetP1Key("Move Right"), std::make_unique<MoveRightCommand>(), InputType::DOWN);
      player1Handler.bindKey(sm.GetP1Key("Climb"), std::make_unique<ClimbCommand>(), InputType::DOWN);
      player1Handler.bindKey(sm.GetP1Key("Crouch"), std::make_unique<CrouchCommand>(), InputType::DOWN);
      player1Handler.bindKey(sm.GetP1Key("Crouch"), std::make_unique<StopCrouchCommand>(), InputType::RELEASED);
      player1Handler.bindKey(sm.GetP1Key("Move Left"), std::make_unique<StopLeftCommand>(), InputType::RELEASED);
      player1Handler.bindKey(sm.GetP1Key("Move Right"), std::make_unique<StopRightCommand>(), InputType::RELEASED);
      
      player1Handler.bindKey(sm.GetP1Key("Attack"), std::make_unique<AttackCommand>(), InputType::PRESSED);
      player1Handler.bindKey(sm.GetP1Key("Jump"), std::make_unique<JumpCommand>(), InputType::PRESSED);
      player1Handler.bindKey(sm.GetP1Key("Dash"), std::make_unique<UseSkillCommand>("Dash"), InputType::PRESSED);
      player1Handler.bindKey(sm.GetP1Key("LongAttack"), std::make_unique<UseSkillCommand>("LongAttack"), InputType::PRESSED);
      player1Handler.bindKey(sm.GetP1Key("SpecialAttack"), std::make_unique<UseSkillCommand>("SpecialAttack"), InputType::PRESSED);
      player1Handler.bindKey(sm.GetP1Key("Block"), std::make_unique<UseSkillCommand>("Block"), InputType::PRESSED);
      player1Handler.bindKey(sm.GetP1Key("Interact"), std::make_unique<InteractCommand>(), InputType::PRESSED);
    }

    player2 = PlayerFactory::createPlayer(p2Name, {0, 0});
    if (player2) {
      Vector2 spawn2 = spawns.size() > 1 ? spawns[1] : Vector2{220.0f, 208.0f};
      player2->setPosition(spawn2);
      player2->setStartPosition(spawn2);
      player2->setCommandQueue(&spawnQueue);
      player2->setPartyInventory(partyInventory);
      
      auto& sm = SettingsManager::GetInstance();
      player2Handler.bindKey(sm.GetP2Key("Move Left"), std::make_unique<MoveLeftCommand>(), InputType::DOWN);
      player2Handler.bindKey(sm.GetP2Key("Move Right"), std::make_unique<MoveRightCommand>(), InputType::DOWN);
      player2Handler.bindKey(sm.GetP2Key("Climb"), std::make_unique<ClimbCommand>(), InputType::DOWN);
      player2Handler.bindKey(sm.GetP2Key("Crouch"), std::make_unique<CrouchCommand>(), InputType::DOWN);
      player2Handler.bindKey(sm.GetP2Key("Crouch"), std::make_unique<StopCrouchCommand>(), InputType::RELEASED);
      player2Handler.bindKey(sm.GetP2Key("Move Left"), std::make_unique<StopLeftCommand>(), InputType::RELEASED);
      player2Handler.bindKey(sm.GetP2Key("Move Right"), std::make_unique<StopRightCommand>(), InputType::RELEASED);
      
      player2Handler.bindKey(sm.GetP2Key("Attack"), std::make_unique<AttackCommand>(), InputType::PRESSED);
      player2Handler.bindKey(sm.GetP2Key("Jump"), std::make_unique<JumpCommand>(), InputType::PRESSED);
      player2Handler.bindKey(sm.GetP2Key("Dash"), std::make_unique<UseSkillCommand>("Dash"), InputType::PRESSED);
      player2Handler.bindKey(sm.GetP2Key("LongAttack"), std::make_unique<UseSkillCommand>("LongAttack"), InputType::PRESSED);
      player2Handler.bindKey(sm.GetP2Key("SpecialAttack"), std::make_unique<UseSkillCommand>("SpecialAttack"), InputType::PRESSED);
      player2Handler.bindKey(sm.GetP2Key("Block"), std::make_unique<UseSkillCommand>("Block"), InputType::PRESSED);
      player2Handler.bindKey(sm.GetP2Key("Interact"), std::make_unique<InteractCommand>(), InputType::PRESSED);
    }

    // CombatSystem is now stateless, so no registerEntity calls here
    // Load item atlas (once globally, no-op if already loaded)
    ItemAtlasRegistry::getInstance().loadAll("assets/maps/item/");

    // Spawn items from LDtk entity data
    activeItems.clear();
    auto entityData = map.GetEntityData();
    for (const auto &data : entityData) {
      auto item =
          ItemFactory::create(data.identifier, data.px, data.fieldInstances);
      if (item) {
        item->setIid(data.iid);
        item->setCommandQueue(&spawnQueue); // <-- FIX
        // Restore persisted state if this item was visited before
        if (!data.iid.empty()) {
          auto it = persistedItemStates.find(data.iid);
          if (it != persistedItemStates.end()) {
            item->getRuntimeStatsMutable(); // ensure access
            item->setItemState(it->second);
          }
        }
        activeItems.push_back(std::move(item));
      }
    }
    std::cout << "[BaseLevelState] Spawned " << activeItems.size()
              << " items.\n";

  } else {
    std::cerr << "[BaseLevelState] Error loading " << mapFilePath << "!\n";
  }
}

// =============================================================================
// [NEW] Constructor cho chế độ Test Play từ MapEditorState
// =============================================================================
BaseLevelState::BaseLevelState(const CustomMapData& customMap,
                               const std::string &p1Name,
                               const std::string &p2Name)
    : mapCamera(416.0f), currentLevel(customMap.name), mapFilePath("custom") {

  bool loaded = map.LoadCustomMap(customMap);
  if (loaded && map.GetHeight() > 0) {
    std::cout << "[BaseLevelState] Loaded custom map ("
              << currentLevel << ") successfully!\n";

    auto spawns = map.GetPlayerSpawns();
    Vector2 spawn1 = spawns.size() > 0 ? spawns[0] : Vector2{180.0f, 208.0f};

    partyInventory = std::make_shared<PartyInventory>();

    player1 = PlayerFactory::createPlayer(p1Name, {0, 0});
    if (player1) {
      player1->setPosition(spawn1);
      player1->setStartPosition(spawn1);
      player1->setCommandQueue(&spawnQueue);
      player1->setPartyInventory(partyInventory);
      
      auto& sm = SettingsManager::GetInstance();
      player1Handler.bindKey(sm.GetP1Key("Move Left"), std::make_unique<MoveLeftCommand>(), InputType::DOWN);
      player1Handler.bindKey(sm.GetP1Key("Move Right"), std::make_unique<MoveRightCommand>(), InputType::DOWN);
      player1Handler.bindKey(sm.GetP1Key("Climb"), std::make_unique<ClimbCommand>(), InputType::DOWN);
      player1Handler.bindKey(sm.GetP1Key("Crouch"), std::make_unique<CrouchCommand>(), InputType::DOWN);
      player1Handler.bindKey(sm.GetP1Key("Crouch"), std::make_unique<StopCrouchCommand>(), InputType::RELEASED);
      player1Handler.bindKey(sm.GetP1Key("Move Left"), std::make_unique<StopLeftCommand>(), InputType::RELEASED);
      player1Handler.bindKey(sm.GetP1Key("Move Right"), std::make_unique<StopRightCommand>(), InputType::RELEASED);
      
      player1Handler.bindKey(sm.GetP1Key("Attack"), std::make_unique<AttackCommand>(), InputType::PRESSED);
      player1Handler.bindKey(sm.GetP1Key("Jump"), std::make_unique<JumpCommand>(), InputType::PRESSED);
      player1Handler.bindKey(sm.GetP1Key("Dash"), std::make_unique<UseSkillCommand>("Dash"), InputType::PRESSED);
      player1Handler.bindKey(sm.GetP1Key("LongAttack"), std::make_unique<UseSkillCommand>("LongAttack"), InputType::PRESSED);
      player1Handler.bindKey(sm.GetP1Key("SpecialAttack"), std::make_unique<UseSkillCommand>("SpecialAttack"), InputType::PRESSED);
      player1Handler.bindKey(sm.GetP1Key("Block"), std::make_unique<UseSkillCommand>("Block"), InputType::PRESSED);
      player1Handler.bindKey(sm.GetP1Key("Interact"), std::make_unique<InteractCommand>(), InputType::PRESSED);
    }

    player2 = PlayerFactory::createPlayer(p2Name, {0, 0});
    if (player2) {
      Vector2 spawn2 = spawns.size() > 1 ? spawns[1] : Vector2{220.0f, 208.0f};
      player2->setPosition(spawn2);
      player2->setStartPosition(spawn2);
      player2->setCommandQueue(&spawnQueue);
      player2->setPartyInventory(partyInventory);
      
      auto& sm = SettingsManager::GetInstance();
      player2Handler.bindKey(sm.GetP2Key("Move Left"), std::make_unique<MoveLeftCommand>(), InputType::DOWN);
      player2Handler.bindKey(sm.GetP2Key("Move Right"), std::make_unique<MoveRightCommand>(), InputType::DOWN);
      player2Handler.bindKey(sm.GetP2Key("Climb"), std::make_unique<ClimbCommand>(), InputType::DOWN);
      player2Handler.bindKey(sm.GetP2Key("Crouch"), std::make_unique<CrouchCommand>(), InputType::DOWN);
      player2Handler.bindKey(sm.GetP2Key("Crouch"), std::make_unique<StopCrouchCommand>(), InputType::RELEASED);
      player2Handler.bindKey(sm.GetP2Key("Move Left"), std::make_unique<StopLeftCommand>(), InputType::RELEASED);
      player2Handler.bindKey(sm.GetP2Key("Move Right"), std::make_unique<StopRightCommand>(), InputType::RELEASED);
      
      player2Handler.bindKey(sm.GetP2Key("Attack"), std::make_unique<AttackCommand>(), InputType::PRESSED);
      player2Handler.bindKey(sm.GetP2Key("Jump"), std::make_unique<JumpCommand>(), InputType::PRESSED);
      player2Handler.bindKey(sm.GetP2Key("Dash"), std::make_unique<UseSkillCommand>("Dash"), InputType::PRESSED);
      player2Handler.bindKey(sm.GetP2Key("LongAttack"), std::make_unique<UseSkillCommand>("LongAttack"), InputType::PRESSED);
      player2Handler.bindKey(sm.GetP2Key("SpecialAttack"), std::make_unique<UseSkillCommand>("SpecialAttack"), InputType::PRESSED);
      player2Handler.bindKey(sm.GetP2Key("Block"), std::make_unique<UseSkillCommand>("Block"), InputType::PRESSED);
      player2Handler.bindKey(sm.GetP2Key("Interact"), std::make_unique<InteractCommand>(), InputType::PRESSED);
    }

    ItemAtlasRegistry::getInstance().loadAll("assets/maps/item/");

    activeItems.clear();
    auto entityData = map.GetEntityData();
    for (const auto &data : entityData) {
      auto item =
          ItemFactory::create(data.identifier, data.px, data.fieldInstances);
      if (item) {
        item->setIid(data.iid);
        item->setCommandQueue(&spawnQueue);
        activeItems.push_back(std::move(item));
      }
    }
    std::cout << "[BaseLevelState] Spawned " << activeItems.size()
              << " items.\n";

  } else {
    std::cerr << "[BaseLevelState] Error loading custom map data!\n";
  }
}


void BaseLevelState::HandleInput() {
  float dt = GetFrameTime();
  if (player1) {
    auto commands = player1Handler.handleInput();
    for (auto *cmd : commands)
      cmd->Execute(*player1);
  }
  if (player2) {
    auto commands = player2Handler.handleInput();
    for (auto *cmd : commands)
      cmd->Execute(*player2);
  }
}

void BaseLevelState::Process() {
  float dt = GetFrameTime();

  if (respawnTimer > 0.0f) {
    respawnTimer -= dt;
    if (respawnTimer <= 0.0f) {
      respawnTimer = -1.0f;
      
      if (SaveManager::getInstance().hasCheckpoint()) {
          restoreFromSaveData(SaveManager::getInstance().getCheckpoint());
      } else {
          // If no checkpoint, we could reload the map entirely from scratch
          // But for now, we'll just reload the current level from scratch
          map.LoadLDtkMap(mapFilePath, currentLevel);
          auto spawns = map.GetPlayerSpawns();
          if (player1) { player1->setPosition(spawns.size() > 0 ? spawns[0] : Vector2{180, 208}); player1->getRuntimeStatsMutable().health = player1->getBaseStats().maxHealth; }
          if (player2) { player2->setPosition(spawns.size() > 1 ? spawns[1] : Vector2{220, 208}); player2->getRuntimeStatsMutable().health = player2->getBaseStats().maxHealth; }
      }

      if (player1) {
          player1->forceState(player1->idleState);
          player1->getRuntimeStatsMutable().velocity = {0,0};
          player1->clearEffects();
      }
      if (player2) {
          player2->forceState(player2->idleState);
          player2->getRuntimeStatsMutable().velocity = {0,0};
          player2->clearEffects();
      }
    }
  } else {
    auto checkDeathCondition = [&](Player *p) {
      if (!p)
        return false;
      if (p->getRuntimeStats().health <= 0)
        return true;
      if (p->getWorldStats().position.y > map.GetHeight() + 32.0f)
        return true;
      return false;
    };

    if (checkDeathCondition(player1.get()) ||
        checkDeathCondition(player2.get())) {
      if (player1->getRuntimeStats().health > 0)
        player1->takeDamage(9999);
      if (player2->getRuntimeStats().health > 0)
        player2->takeDamage(9999);
      respawnTimer = 1.5f;
    }
  }

  if (!nextLevelToLoad.empty()) {
    TransitionToLevel(nextLevelToLoad, nextLevelDir, triggerGlobalX,
                      triggerGlobalY);
    nextLevelToLoad = "";
  }

  if (player1 && player2) {
    mapCamera.UpdateMultiplayer(player1->getWorldStats().position,
                                player2->getWorldStats().position,
                                map.GetWidth(), map.GetHeight(), dt);
  } else if (player1) {
    mapCamera.Update(player1->getWorldStats().position, map.GetWidth(),
                     map.GetHeight(), dt);
  } else if (player2) {
    mapCamera.Update(player2->getWorldStats().position, map.GetWidth(),
                     map.GetHeight(), dt);
  }

  // Handle explosion damage: burn all entities (players + NPCs) inside blast
  // rect
  auto explosionCmds =
      spawnQueue.peekAndConsumeByCategory(SpawnCategory::ExplosionDamage);
  for (const auto &cmd : explosionCmds) {
    const Rectangle &blast = cmd.explosionRect;
    auto applyBurn = [&](Entity *e) {
      if (!e || !e->getIsActive())
        return;
      if (CheckCollisionRecs(e->getHitbox(), blast)) {
        e->takeDamage(30);                          // Instant explosion damage
        auto burn = std::make_unique<LavaEffect>(); // Chữ hoa L cho LavaEffect
        burn->setInLava(
            false); // Chú ý: Hàm này tên là setInLava, không phải setInBurn
        e->addEffect(std::move(burn));
        std::cout << "[Explosion] Damage and Burn applied!\n";
      }
    };
    applyBurn(player1.get());
    applyBurn(player2.get());
    for (auto &ent : activeEntities)
      applyBurn(ent.get());
  }

  // Cleanups
  activeEntities.erase(std::remove_if(activeEntities.begin(),
                                      activeEntities.end(),
                                      [&](const std::unique_ptr<Entity> &e) {
                                        if (!e->getIsActive()) {
                                            if (!e->getIid().empty()) {
                                                persistedDeadEntities.insert(e->getIid());
                                            }
                                            return true;
                                        }
                                        return false;
                                      }),
                       activeEntities.end());
}

void BaseLevelState::Update(float dt) {
  std::vector<Rectangle> dynamicSolids;
  for (const auto &item : activeItems) {
    if (!item->getIsActive())
      continue;
    Rectangle r = item->getSolidRect();
    if (r.width > 0 && r.height > 0)
      dynamicSolids.push_back(r);
  }

    if (player1) {
      player1->updatePhysicsWithMap(map, dynamicSolids, dt);
      player1->updateStateFromPhysics();
      player1->update(dt);
    }
    if (player2) {
      player2->updatePhysicsWithMap(map, dynamicSolids, dt);
      player2->updateStateFromPhysics();
      player2->update(dt);
    }

    // Player-to-Player Pushing Resolution
    if (player1 && player2 && player1->getIsActive() && player2->getIsActive()) {
        Rectangle r1 = player1->getHitbox();
        Rectangle r2 = player2->getHitbox();
        if (CheckCollisionRecs(r1, r2)) {
            float center1 = r1.x + r1.width / 2.0f;
            float center2 = r2.x + r2.width / 2.0f;
            float overlap = (r1.width + r2.width) / 2.0f - std::abs(center1 - center2);
            
            if (overlap > 0) {
                float push = overlap / 2.0f + 0.1f; // +0.1f epsilon to fully separate
                
                Vector2 p1Pos = player1->getWorldStats().position;
                Vector2 p2Pos = player2->getWorldStats().position;
                
                float dir1 = (center1 < center2) ? -1.0f : 1.0f;
                float dir2 = -dir1;

                // Check if P1 hits a wall if pushed
                Rectangle nextR1 = r1;
                nextR1.x += push * dir1;
                bool p1HitsWall = false;
                for (const auto& tile : map.GetCollidingTiles(nextR1)) {
                    if (tile.type == CollisionType::Solid) { p1HitsWall = true; break; }
                }

                // Check if P2 hits a wall if pushed
                Rectangle nextR2 = r2;
                nextR2.x += push * dir2;
                bool p2HitsWall = false;
                for (const auto& tile : map.GetCollidingTiles(nextR2)) {
                    if (tile.type == CollisionType::Solid) { p2HitsWall = true; break; }
                }

                if (!p1HitsWall && !p2HitsWall) {
                    player1->setPosition({p1Pos.x + push * dir1, p1Pos.y});
                    player2->setPosition({p2Pos.x + push * dir2, p2Pos.y});
                } else if (p1HitsWall && !p2HitsWall) {
                    // P1 is against a wall, push P2 the full amount
                    player2->setPosition({p2Pos.x + overlap * dir2 + 0.1f, p2Pos.y});
                } else if (!p1HitsWall && p2HitsWall) {
                    // P2 is against a wall, push P1 the full amount
                    player1->setPosition({p1Pos.x + overlap * dir1 + 0.1f, p1Pos.y});
                }
            }
        }
    }

    std::string next = "";
  std::string dir = "";
  float edge = 16.0f;
  float tX = 0, tY = 0;

  auto checkEdge = [&](Player *p) {
    if (!p || !next.empty())
      return;
    float pX = p->getWorldStats().position.x;
    float pY = p->getWorldStats().position.y;
    float globalX = pX + map.GetWorldX();
    float globalY = pY + map.GetWorldY();

    if (pX > map.GetWidth() - edge) {
      dir = "e";
      next = map.GetNeighbour(dir, globalX, globalY);
    } else if (pX < edge) {
      dir = "w";
      next = map.GetNeighbour(dir, globalX, globalY);
    } else if (pY > map.GetHeight() - edge) {
      dir = "s";
      next = map.GetNeighbour(dir, globalX, globalY);
    } else if (pY < edge) {
      dir = "n";
      next = map.GetNeighbour(dir, globalX, globalY);
    }

    if (!next.empty()) {
      tX = globalX;
      tY = globalY;
    }
  };

  checkEdge(player1.get());
  checkEdge(player2.get());

  if (!next.empty()) {
    nextLevelToLoad = next;
    nextLevelDir = dir;
    triggerGlobalX = tX;
    triggerGlobalY = tY;
  }

  for (auto &entity : activeEntities) {
    entity->updatePhysicsWithMap(map, dynamicSolids, dt);
    entity->update(dt);
  }

  // Update items and check player overlap
  for (auto &item : activeItems) {
    if (!item->getIsActive())
      continue;
    item->update(dt);

    if (auto coin = dynamic_cast<Coin*>(item.get())) {
        Player* targetPlayer = nullptr;
        float minDst = 160.0f; // Pull radius (5 blocks)
        
        auto checkMagnet = [&](Player* p) {
            if (p && p->getBuffManager().hasGoldMagnet()) {
                float dst = Vector2Distance(p->getWorldStats().position, coin->getWorldStats().position);
                if (dst < minDst) {
                    minDst = dst;
                    targetPlayer = p;
                }
            }
        };
        
        checkMagnet(player1.get());
        checkMagnet(player2.get());

        if (targetPlayer) {
            Vector2 dir = Vector2Normalize(Vector2Subtract(targetPlayer->getWorldStats().position, coin->getWorldStats().position));
            float pullSpeed = 400.0f;
            coin->getRuntimeStatsMutable().velocity.x = dir.x * pullSpeed;
            coin->getRuntimeStatsMutable().velocity.y = dir.y * pullSpeed;
            coin->getBaseStatsMutable().gravityScale = 0.0f;
        } else {
            coin->getBaseStatsMutable().gravityScale = 160.0f;
        }
    }

    // If item is dynamic (has gravity), apply physics so it can fall and bounce
    if (item->getBaseStats().gravityScale > 0.0f || dynamic_cast<Coin*>(item.get())) {
      // Empty dynamicSolids since items don't usually collide with other moving
      // boxes, just the map
      item->updatePhysicsWithMap(map, std::vector<Rectangle>{}, dt);
    }

    Rectangle itemBox = item->getHitbox();
    auto handleInteract = [&](Player* p) {
        if (p && CheckCollisionRecs(itemBox, p->getHitbox())) {
            ItemState oldState = item->getItemState();
            item->onInteract(*p);
            
            // Check if it's a flag that just got activated
            if (oldState != ItemState::Active && item->getItemState() == ItemState::Active && dynamic_cast<Flag*>(item.get()) != nullptr) {
                SaveManager::getInstance().setCheckpoint(createSaveData());
                SaveManager::getInstance().saveToFile("saves/save.json");
            }
        }
    };
    
    handleInteract(player1.get());
    handleInteract(player2.get());
  }

  auto entityCmds = spawnQueue.peekAndConsumeByCategory(SpawnCategory::Entity);
  for (const auto &cmd : entityCmds) {
    if (!cmd.iid.empty() && persistedDeadEntities.count(cmd.iid)) {
        continue; // Skip spawning if it's dead
    }
    auto entity = EntityFactory::create(cmd);
    if (entity) {
      entity->setCommandQueue(&spawnQueue);
      activeEntities.push_back(std::move(entity));
    }
  }

  auto itemCmds = spawnQueue.peekAndConsumeByCategory(SpawnCategory::Item);
  for (const auto &cmd : itemCmds) {
    // Pass velocity for ThrownBoom; other items ignore it
    auto item = ItemFactory::createDynamic(cmd.itemIdentifier, cmd.position,
                                           cmd.velocity);
    if (item) {
      item->setCommandQueue(&spawnQueue);
      activeItems.push_back(std::move(item));
    }
  }

  // Cleanups moved to Process()

  std::vector<Entity*> allEntities;
  if (player1) allEntities.push_back(player1.get());
  if (player2) allEntities.push_back(player2.get());
  for (const auto& e : activeEntities) {
      if (e && e->getIsActive()) allEntities.push_back(e.get());
  }
  combatSystem.update(allEntities, dt);
}

void BaseLevelState::Render(float alpha) const {
  ClearBackground(BLACK);

  mapCamera.BeginMode();
  map.Draw();
  for (const auto &item : activeItems) {
    if (item->getIsActive())
      item->render(alpha);
  }
  for (const auto &entity : activeEntities)
    entity->render(alpha);
  if (player1)
    player1->render(alpha);
  if (player2)
    player2->render(alpha);
  std::vector<Entity*> allEntities;
  if (player1) allEntities.push_back(player1.get());
  if (player2) allEntities.push_back(player2.get());
  for (const auto& e : activeEntities) {
      if (e && e->getIsActive()) allEntities.push_back(e.get());
  }
  combatSystem.renderDebug(allEntities);
  mapCamera.EndMode();

  // Render HUD over the screen
  PlayerHUD::render(player1.get(), player2.get(), partyInventory.get());
}

void BaseLevelState::TransitionToLevel(const std::string &nextLevel,
                                       const std::string &dir,
                                       float triggerGlobalX,
                                       float triggerGlobalY) {
  std::cout << "[BaseLevelState] Transitioning to " << nextLevel << "...\n";

  // Persist item states before clearing
  for (const auto &item : activeItems) {
    if (!item->getIid().empty()) {
      persistedItemStates[item->getIid()] = item->getItemState();
    }
  }

  if (map.LoadLDtkMap(mapFilePath, nextLevel)) {
    currentLevel = nextLevel;
    activeEntities.clear();
    // combatSystem is stateless so no clear needed

    float mapW = (float)map.GetWidth();
    float mapH = (float)map.GetHeight();
    float newWorldX = (float)map.GetWorldX();
    float newWorldY = (float)map.GetWorldY();
    float targetXNew = triggerGlobalX - newWorldX;
    float targetYNew = triggerGlobalY - newWorldY;
    float margin = 64.0f;

    if (dir == "e")
      targetXNew = margin;
    else if (dir == "w")
      targetXNew = mapW - margin;
    else if (dir == "s")
      targetYNew = margin;
    else if (dir == "n")
      targetYNew = mapH - margin;

    player1->setPosition({targetXNew, targetYNew});
    player2->setPosition({targetXNew, targetYNew});

    // Re-spawn items for the new level (restoring persisted states)
    ItemAtlasRegistry::getInstance().loadAll("assets/maps/item/");
    activeItems.clear();
    auto entityData = map.GetEntityData();
    for (const auto &data : entityData) {
      auto item =
          ItemFactory::create(data.identifier, data.px, data.fieldInstances);
      if (item) {
        item->setIid(data.iid);
        item->setCommandQueue(&spawnQueue); // <-- FIX
        if (!data.iid.empty()) {
          auto it = persistedItemStates.find(data.iid);
          if (it != persistedItemStates.end())
            item->setItemState(it->second);
        }
        activeItems.push_back(std::move(item));
      }
    }
  }
}

GameSaveData BaseLevelState::createSaveData() const {
  GameSaveData data;
  data.isValid = true;
  data.levelData.worldId = ""; // Optional for now
  data.levelData.levelId = currentLevel;
  data.levelData.mapFilePath = mapFilePath;
  data.levelData.persistedItemStates = persistedItemStates;
  data.levelData.persistedDeadEntities = persistedDeadEntities;

  if (partyInventory) {
    data.inventory.coins = partyInventory->coins;
    data.inventory.keys = partyInventory->keys;
  }

  auto populatePlayer = [](Player* p, PlayerSaveData& pd) {
    if (p) {
      pd.exists = true;
      pd.posX = p->getWorldStats().position.x;
      pd.posY = p->getWorldStats().position.y;
      pd.health = p->getRuntimeStats().health;
      pd.maxHealth = p->getBaseStats().maxHealth;
      pd.mana = p->getRuntimeStats().mana;
      pd.storedItemSlot = p->getRuntimeStats().storedItemSlot;
      pd.isFacingRight = p->getWorldStats().isFacingRight;
    }
  };

  populatePlayer(player1.get(), data.p1);
  populatePlayer(player2.get(), data.p2);

  return data;
}

void BaseLevelState::restoreFromSaveData(const GameSaveData& data) {
  if (!data.isValid) return;

  // Restore inventory
  if (partyInventory) {
    partyInventory->coins = data.inventory.coins;
    partyInventory->keys = data.inventory.keys;
  }

  // Restore player stats and positions
  auto applyPlayer = [](Player* p, const PlayerSaveData& pd) {
    if (p && pd.exists) {
      p->setPosition({pd.posX, pd.posY});
      p->getWorldStatsMutable().isFacingRight = pd.isFacingRight;
      p->getRuntimeStatsMutable().health = pd.health;
      p->getBaseStatsMutable().maxHealth = pd.maxHealth;
      p->getRuntimeStatsMutable().mana = pd.mana;
      p->getRuntimeStatsMutable().storedItemSlot = pd.storedItemSlot;
    }
  };

  applyPlayer(player1.get(), data.p1);
  applyPlayer(player2.get(), data.p2);

  // Restore Level State
  persistedItemStates = data.levelData.persistedItemStates;
  persistedDeadEntities = data.levelData.persistedDeadEntities;

  // Reload the map from LDtk to reset dynamic objects
  activeEntities.clear();
  activeItems.clear();
  combatSystem = CombatSystem(); // Reset combat system
  // No need to register entities since CombatSystem is stateless

  map.LoadLDtkMap(data.levelData.mapFilePath, data.levelData.levelId);
  currentLevel = data.levelData.levelId;

  // Respawn items based on restored persisted state
  auto entityData = map.GetEntityData();
  for (const auto& d : entityData) {
    auto item = ItemFactory::create(d.identifier, d.px, d.fieldInstances);
    if (item) {
      item->setIid(d.iid);
      item->setCommandQueue(&spawnQueue);
      if (!d.iid.empty()) {
        auto it = persistedItemStates.find(d.iid);
        if (it != persistedItemStates.end()) {
          item->getRuntimeStatsMutable();
          item->setItemState(it->second);
        }
      }
      activeItems.push_back(std::move(item));
    }
  }

  // Active enemies are spawned by CommandQueue during update loop, but we need to ensure their IIDs are checked.
  // We'll update the Entity spawn logic in Process() to check persistedDeadEntities.
}
