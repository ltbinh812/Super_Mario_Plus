#include "BaseLevelState.h"
#include "Effects.h"
#include "EntityFactory.h"
#include "EnemyFactory.h"
#include "GameState.h"
#include "ItemAtlasRegistry.h"
#include "ItemFactory.h"
#include "PlayerCommands.h"
#include "PlayerFactory.h"
#include "SaveManager.h"
#include "Flag.h"
#include "PlayerHUD.h"
#include "Coin.h"
#include <algorithm>
#include <iostream>
#include <raylib.h>
#include <raymath.h>

BaseLevelState::BaseLevelState(const std::string &mapFilePath,
                               const std::string &initialLevel,
                               const std::string &p1Name,
                               const std::string &p2Name)
    : mapCamera(416.0f), currentLevel(initialLevel), mapFilePath(mapFilePath) {

  std::cout << "[BaseLevelState] Loading map: " << mapFilePath << " level: " << initialLevel << "\n";
  if (map.LoadLDtkMap(mapFilePath, initialLevel)) {
    std::cout << "[BaseLevelState] Map loaded successfully!\n";

    auto spawns = map.GetPlayerSpawns();
    Vector2 spawn1 = spawns.size() > 0 ? spawns[0] : Vector2{180.0f, 208.0f};

    partyInventory = std::make_shared<PartyInventory>();

    player1 = PlayerFactory::createPlayer(p1Name, {0, 0});
    if (player1) {
      player1->setPosition(spawn1);
      player1->setStartPosition(spawn1);
      player1->setCommandQueue(&spawnQueue);
      player1->setPartyInventory(partyInventory);
    }

    player2 = PlayerFactory::createPlayer(p2Name, {0, 0});
    if (player2) {
      Vector2 spawn2 = spawns.size() > 1 ? spawns[1] : Vector2{220.0f, 208.0f};
      player2->setPosition(spawn2);
      player2->setStartPosition(spawn2);
      player2->setCommandQueue(&spawnQueue);
      player2->setPartyInventory(partyInventory);
    }

    bindPlayerInputs();

    ItemAtlasRegistry::getInstance().loadAll("assets/maps/item/");
    ItemAtlasRegistry::getInstance().loadAtlas("mob_mushroom", "assets/mobs/mob_mushroom.json", "assets/mobs/mob_mushroom.png");

    spawnEntitiesFromMap();
    TraceLog(LOG_INFO, "[BaseLevelState] Spawned %d items and %d entities.", activeItems.size(), activeEntities.size());
  } else {
    std::cerr << "[BaseLevelState] Error loading " << mapFilePath << "!\n";
  }
}

void BaseLevelState::HandleInput() {
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

  std::vector<Player*> players;
  if (player1) players.push_back(player1.get());
  if (player2) players.push_back(player2.get());

  for (auto& ent : activeEntities) {
      if (ent) {
          ent->setTargetPlayers(players);
          ent->decideAction();
      }
  }
}

void BaseLevelState::Process() {
  float dt = GetFrameTime();

  processDeathCondition(dt);
  processPlayerPushing();
  
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
    TransitionToLevel(next, dir, tX, tY);
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

  for (auto& ent : activeEntities) {
      if (ent) ent->process();
  }

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

  processItemInteractions();
  processSpawnQueue();
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

  for (auto &entity : activeEntities) {
    entity->updatePhysicsWithMap(map, dynamicSolids, dt);
    entity->update(dt);
  }

  for (auto &item : activeItems) {
    if (!item->getIsActive())
      continue;
    item->update(dt);

    if (item->getBaseStats().gravityScale > 0.0f || dynamic_cast<Coin*>(item.get())) {
      item->updatePhysicsWithMap(map, std::vector<Rectangle>{}, dt);
    }
  }

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

  PlayerHUD::render(player1.get(), player2.get(), partyInventory.get());
}

void BaseLevelState::TransitionToLevel(const std::string &nextLevel,
                                       const std::string &dir,
                                       float triggerGlobalX,
                                       float triggerGlobalY) {
  std::cout << "[BaseLevelState] Transitioning to " << nextLevel << "...\n";

  for (const auto &item : activeItems) {
    if (!item->getIid().empty()) {
      persistedItemStates[item->getIid()] = item->getItemState();
    }
  }

  if (map.LoadLDtkMap(mapFilePath, nextLevel)) {
    currentLevel = nextLevel;
    activeEntities.clear();
    activeItems.clear();

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

    spawnEntitiesFromMap();
  }
}

GameSaveData BaseLevelState::createSaveData() const {
  GameSaveData data;
  data.isValid = true;
  data.levelData.worldId = "";
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

  if (partyInventory) {
    partyInventory->coins = data.inventory.coins;
    partyInventory->keys = data.inventory.keys;
  }

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

  persistedItemStates = data.levelData.persistedItemStates;
  persistedDeadEntities = data.levelData.persistedDeadEntities;

  activeEntities.clear();
  activeItems.clear();
  combatSystem = CombatSystem();

  map.LoadLDtkMap(data.levelData.mapFilePath, data.levelData.levelId);
  currentLevel = data.levelData.levelId;

  spawnEntitiesFromMap();
}

void BaseLevelState::spawnEntitiesFromMap() {
    activeItems.clear();
    auto entityData = map.GetEntityData();
    for (const auto &data : entityData) {
        if (data.identifier.rfind("Mob_", 0) == 0 || data.identifier.rfind("Boss_", 0) == 0) {
            if (!data.iid.empty() && persistedDeadEntities.find(data.iid) != persistedDeadEntities.end()) {
                continue;
            }
            auto enemy = EnemyFactory::create(data.identifier, data.px, data.fieldInstances);
            if (enemy) {
                enemy->setIid(data.iid);
                enemy->setCommandQueue(&spawnQueue);
                activeEntities.push_back(std::move(enemy));
            }
        } else {
            auto item = ItemFactory::create(data.identifier, data.px, data.fieldInstances);
            if (item) {
                item->setIid(data.iid);
                item->setCommandQueue(&spawnQueue);
                if (!data.iid.empty()) {
                    auto it = persistedItemStates.find(data.iid);
                    if (it != persistedItemStates.end()) {
                        item->getRuntimeStatsMutable();
                        item->setItemState(it->second);
                    }
                }
                activeItems.push_back(std::move(item));
            }
        }
    }
}

void BaseLevelState::bindPlayerInputs() {
    if (player1) {
      player1Handler.bindKey(KEY_A, std::make_unique<MoveLeftCommand>(), InputType::DOWN);
      player1Handler.bindKey(KEY_D, std::make_unique<MoveRightCommand>(), InputType::DOWN);
      player1Handler.bindKey(KEY_W, std::make_unique<ClimbCommand>(), InputType::DOWN);
      player1Handler.bindKey(KEY_S, std::make_unique<CrouchCommand>(), InputType::DOWN);
      player1Handler.bindKey(KEY_S, std::make_unique<StopCrouchCommand>(), InputType::RELEASED);
      player1Handler.bindKey(KEY_A, std::make_unique<StopLeftCommand>(), InputType::RELEASED);
      player1Handler.bindKey(KEY_D, std::make_unique<StopRightCommand>(), InputType::RELEASED);
      player1Handler.bindKey(KEY_J, std::make_unique<AttackCommand>(), InputType::PRESSED);
      player1Handler.bindKey(KEY_K, std::make_unique<JumpCommand>(), InputType::PRESSED);
      player1Handler.bindKey(KEY_L, std::make_unique<UseSkillCommand>("Dash"), InputType::PRESSED);
      player1Handler.bindKey(KEY_U, std::make_unique<UseSkillCommand>("LongAttack"), InputType::PRESSED);
      player1Handler.bindKey(KEY_I, std::make_unique<UseSkillCommand>("SpecialAttack"), InputType::PRESSED);
      player1Handler.bindKey(KEY_Q, std::make_unique<UseSkillCommand>("Block"), InputType::PRESSED);
      player1Handler.bindKey(KEY_E, std::make_unique<InteractCommand>(), InputType::PRESSED);
    }

    if (player2) {
      player2Handler.bindKey(KEY_LEFT, std::make_unique<MoveLeftCommand>(), InputType::DOWN);
      player2Handler.bindKey(KEY_RIGHT, std::make_unique<MoveRightCommand>(), InputType::DOWN);
      player2Handler.bindKey(KEY_UP, std::make_unique<ClimbCommand>(), InputType::DOWN);
      player2Handler.bindKey(KEY_DOWN, std::make_unique<CrouchCommand>(), InputType::DOWN);
      player2Handler.bindKey(KEY_DOWN, std::make_unique<StopCrouchCommand>(), InputType::RELEASED);
      player2Handler.bindKey(KEY_LEFT, std::make_unique<StopLeftCommand>(), InputType::RELEASED);
      player2Handler.bindKey(KEY_RIGHT, std::make_unique<StopRightCommand>(), InputType::RELEASED);
      player2Handler.bindKey(KEY_KP_1, std::make_unique<AttackCommand>(), InputType::PRESSED);
      player2Handler.bindKey(KEY_KP_2, std::make_unique<JumpCommand>(), InputType::PRESSED);
      player2Handler.bindKey(KEY_KP_3, std::make_unique<UseSkillCommand>("Dash"), InputType::PRESSED);
      player2Handler.bindKey(KEY_KP_4, std::make_unique<UseSkillCommand>("LongAttack"), InputType::PRESSED);
      player2Handler.bindKey(KEY_KP_5, std::make_unique<UseSkillCommand>("SpecialAttack"), InputType::PRESSED);
      player2Handler.bindKey(KEY_RIGHT_SHIFT, std::make_unique<UseSkillCommand>("Block"), InputType::PRESSED);
      player2Handler.bindKey(KEY_ENTER, std::make_unique<InteractCommand>(), InputType::PRESSED);
    }
}

void BaseLevelState::processDeathCondition(float dt) {
  if (respawnTimer > 0.0f) {
    respawnTimer -= dt;
    if (respawnTimer <= 0.0f) {
      respawnTimer = -1.0f;
      
      if (SaveManager::getInstance().hasCheckpoint()) {
          restoreFromSaveData(SaveManager::getInstance().getCheckpoint());
          if (player1) player1->respawn(player1->getWorldStats().position);
          if (player2) player2->respawn(player2->getWorldStats().position);
      } else {
          map.LoadLDtkMap(mapFilePath, currentLevel);
          auto spawns = map.GetPlayerSpawns();
          if (player1) player1->respawn(spawns.size() > 0 ? spawns[0] : Vector2{180.0f, 208.0f});
          if (player2) player2->respawn(spawns.size() > 1 ? spawns[1] : Vector2{220.0f, 208.0f});
      }
    }
  } else {
    auto checkDeathCondition = [&](Player *p) {
      return p && (p->isDead() || p->isOutOfBounds(map.GetHeight() + 32.0f));
    };

    if (checkDeathCondition(player1.get()) ||
        checkDeathCondition(player2.get())) {
      if (player1 && !player1->isDead())
        player1->takeDamage(9999);
      if (player2 && !player2->isDead())
        player2->takeDamage(9999);
      respawnTimer = 1.5f;
    }
  }
}

void BaseLevelState::processPlayerPushing() {
  if (player1 && player2 && player1->getIsActive() && player2->getIsActive()) {
      Rectangle r1 = player1->getHitbox();
      Rectangle r2 = player2->getHitbox();
      if (CheckCollisionRecs(r1, r2)) {
          float center1 = r1.x + r1.width / 2.0f;
          float center2 = r2.x + r2.width / 2.0f;
          float overlap = (r1.width + r2.width) / 2.0f - std::abs(center1 - center2);
          
          if (overlap > 0) {
              float push = overlap / 2.0f + 0.1f;
              
              Vector2 p1Pos = player1->getWorldStats().position;
              Vector2 p2Pos = player2->getWorldStats().position;
              
              float dir1 = (center1 < center2) ? -1.0f : 1.0f;
              float dir2 = -dir1;

              Rectangle nextR1 = r1;
              nextR1.x += push * dir1;
              bool p1HitsWall = false;
              for (const auto& tile : map.GetCollidingTiles(nextR1)) {
                  if (tile.type == CollisionType::Solid) { p1HitsWall = true; break; }
              }

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
                  player2->setPosition({p2Pos.x + overlap * dir2 + 0.1f, p2Pos.y});
              } else if (!p1HitsWall && p2HitsWall) {
                  player1->setPosition({p1Pos.x + overlap * dir1 + 0.1f, p1Pos.y});
              }
          }
      }
  }
}


void BaseLevelState::processItemInteractions() {
  for (auto &item : activeItems) {
    if (!item->getIsActive())
      continue;
    
    item->process({player1.get(), player2.get()});

    Rectangle itemBox = item->getHitbox();
    auto handleInteract = [&](Player* p) {
        if (p && CheckCollisionRecs(itemBox, p->getHitbox())) {
            ItemState oldState = item->getItemState();
            item->onInteract(*p);
            
            if (oldState != ItemState::Active && item->getItemState() == ItemState::Active && dynamic_cast<Flag*>(item.get()) != nullptr) {
                SaveManager::getInstance().setCheckpoint(createSaveData());
                SaveManager::getInstance().saveToFile("save.json");
            }
        }
    };
    
    handleInteract(player1.get());
    handleInteract(player2.get());
  }
}

void BaseLevelState::processSpawnQueue() {
  auto entityCmds = spawnQueue.peekAndConsumeByCategory(SpawnCategory::Entity);
  for (const auto &cmd : entityCmds) {
    if (!cmd.iid.empty() && persistedDeadEntities.count(cmd.iid)) {
        continue;
    }
    auto entity = EntityFactory::create(cmd);
    if (entity) {
      entity->setCommandQueue(&spawnQueue);
      activeEntities.push_back(std::move(entity));
    }
  }

  auto itemCmds = spawnQueue.peekAndConsumeByCategory(SpawnCategory::Item);
  for (const auto &cmd : itemCmds) {
    auto item = ItemFactory::createDynamic(cmd.itemIdentifier, cmd.position, cmd.velocity);
    if (item) {
      item->setCommandQueue(&spawnQueue);
      activeItems.push_back(std::move(item));
    }
  }
}
