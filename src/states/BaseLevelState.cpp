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
#include "DialogueRegistry.h"
#include <algorithm>
#include <iostream>
#include <raylib.h>
#include <raymath.h>

BaseLevelState::BaseLevelState(const std::string &mapFilePath,
                               const std::string &initialLevel,
                               const std::string &p1Name)
    : mapCamera(600.0f), currentLevel(initialLevel), mapFilePath(mapFilePath) {

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



    bindPlayerInputs();

    ItemAtlasRegistry::getInstance().loadAll("assets/maps/item/");
    ItemAtlasRegistry::getInstance().loadAtlas("mob_mushroom", "assets/mobs/mob_mushroom.json", "assets/mobs/mob_mushroom.png");

    spawnEntitiesFromMap();
    spawnCutsceneTriggersFromMap();
    TraceLog(LOG_INFO, "[BaseLevelState] Spawned %d items and %d entities.", activeItems.size(), activeEntities.size());
  } else {
    std::cerr << "[BaseLevelState] Error loading " << mapFilePath << "!\n";
  }
}

void BaseLevelState::HandleInput() {
  // Khi cutscene active: chỉ forward input cho cutscene, BLOCK input player
  // Nhưng entity AI vẫn chạy bình thường (decideAction ở dưới)
  if (cutsceneManager.isActive()) {
    cutsceneManager.handleInput();
    
    // Stop grounded players so they don't run endlessly during cutscenes
    // We let mid-air players continue their jump arcs until they land
    if (player1 && player1->getRuntimeStats().isGrounded) {
        player1->stopLeftRun();
        player1->stopRightRun();
    }
  } else {
    if (player1) {
      auto commands = player1Handler.handleInput();
      for (auto *cmd : commands)
        cmd->Execute(*player1);
    }
  }

  // Entity AI vẫn chạy bình thường kể cả khi cutscene đang diễn ra
  std::vector<Player*> players;
  if (player1) players.push_back(player1.get());

  for (auto& ent : activeEntities) {
      if (ent) {
          ent->setTargetPlayers(players);
          ent->decideAction();
      }
  }
}

void BaseLevelState::Process() {
  float dt = GetFrameTime();

  // Cutscene process luôn chạy (kiểm tra phase transition)
  if (cutsceneManager.isActive()) {
    cutsceneManager.process();
  }
  
  if (cutsceneManager.justFinished()) {
      std::string finishedId = cutsceneManager.getFinishedCutsceneId();
      for (auto& ent : activeEntities) {
          if (ent) ent->onCutsceneEnd(finishedId);
      }
      if (player1) player1->onCutsceneEnd(finishedId);
  }

  // Gameplay logic vẫn chạy bình thường kể cả khi cutscene đang diễn ra
  processDeathCondition(dt);
  
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

  if (!next.empty()) {
    TransitionToLevel(next, dir, tX, tY);
  }

  if (!cutsceneManager.isActive()) {
    if (player1) {
      mapCamera.Update(player1->getWorldStats().position, player1->getRuntimeStats().velocity, map.GetWidth(),
                       map.GetHeight(), dt);
    }
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
  processCutsceneTriggers();
}

void BaseLevelState::Update(float dt) {
  // Cutscene update luôn chạy (camera mode, dialogue timer)
  if (cutsceneManager.isActive()) {
    cutsceneManager.update(dt);
  }

  // Gameplay physics vẫn chạy bình thường
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
    
  std::vector<Entity*> allEntities;
  if (player1) allEntities.push_back(player1.get());
  for (const auto& e : activeEntities) {
      if (e && e->getIsActive()) allEntities.push_back(e.get());
  }
  combatSystem.renderDebug(allEntities);

  for (const auto& trigger : cutsceneTriggers) {
      trigger.renderDebug();
  }
  
  mapCamera.EndMode();

  PlayerHUD::render(player1.get(), partyInventory.get());

  // Cutscene dialogue box vẽ trên cùng (screen space, ngoài camera)
  if (cutsceneManager.isActive()) {
    cutsceneManager.render(alpha);
  }
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

    spawnEntitiesFromMap();
    spawnCutsceneTriggersFromMap();
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

}

void BaseLevelState::processDeathCondition(float dt) {
  if (respawnTimer > 0.0f) {
    respawnTimer -= dt;
    if (respawnTimer <= 0.0f) {
      respawnTimer = -1.0f;
      
      if (SaveManager::getInstance().hasCheckpoint()) {
          restoreFromSaveData(SaveManager::getInstance().getCheckpoint());
          if (player1) player1->respawn(player1->getWorldStats().position);
      } else {
          map.LoadLDtkMap(mapFilePath, currentLevel);
          auto spawns = map.GetPlayerSpawns();
          if (player1) player1->respawn(spawns.size() > 0 ? spawns[0] : Vector2{180.0f, 208.0f});
      }
    }
  } else {
    auto checkDeathCondition = [&](Player *p) {
      return p && (p->isDead() || p->isOutOfBounds(map.GetHeight() + 32.0f));
    };

    if (checkDeathCondition(player1.get())) {
      if (player1 && !player1->isDead())
        player1->takeDamage(9999);
      respawnTimer = 1.5f;
    }
  }
}


void BaseLevelState::processItemInteractions() {
  for (auto &item : activeItems) {
    if (!item->getIsActive())
      continue;
    
    item->process({player1.get()});

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

void BaseLevelState::processCutsceneTriggers() {
  // Không check trigger khi cutscene đang active
  if (cutsceneManager.isActive()) return;

  for (auto& trigger : cutsceneTriggers) {
    auto checkPlayer = [&](Player* p) {
      if (!p || !p->getIsActive()) return;
      Rectangle hitbox = p->getHitbox();
      if (trigger.checkTrigger(hitbox)) {
        // Lấy vị trí Player hiện tại để camera quay về sau
        Vector2 playerPos = p->getWorldStats().position;
        cutsceneManager.startCutscene(trigger.getScript(), mapCamera, playerPos);
        trigger.markTriggered();
        
        // Notify entities
        std::string tId = trigger.getTriggerId();
        for (auto& ent : activeEntities) {
            if (ent) ent->onCutsceneStart(tId);
        }
        if (player1) player1->onCutsceneStart(tId);
      }
    };

    checkPlayer(player1.get());
    // Dừng check nếu đã bắt đầu cutscene
    if (cutsceneManager.isActive()) break;
  }
}

void BaseLevelState::spawnCutsceneTriggersFromMap() {
  cutsceneTriggers.clear();
  auto entityData = map.GetEntityData();

  for (const auto& data : entityData) {
    // Tìm entity type "CutsceneTrigger" từ LDtk
    if (data.identifier == "CutsceneTrigger") {
      Vector2 size = {data.width, data.height};

      CutsceneTrigger trigger(data.px, size, data.fieldInstances, map.GetWorldScale());

      // Load dialogue file nếu có dialogueId
      const std::string& dialogueId = trigger.getScript().dialogueId;
      if (!dialogueId.empty() && !DialogueRegistry::getInstance().has(dialogueId)) {
        std::string path = "assets/dialogues/" + dialogueId + ".json";
        DialogueRegistry::getInstance().loadFromFile(path);
      }

      // Khôi phục trạng thái oneShot từ cutsceneManager
      if (cutsceneManager.isTriggered(trigger.getTriggerId())) {
        trigger.setHasTriggered(true);
      }

      cutsceneTriggers.push_back(std::move(trigger));
    }
  }

  TraceLog(LOG_INFO, "[BaseLevelState] Spawned %d cutscene triggers.", (int)cutsceneTriggers.size());
}
