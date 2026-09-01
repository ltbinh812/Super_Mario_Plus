#include "BaseLevelState.h"
#include "MainMenuState.h"
#include "StateCommands.h"
#include "EndgameState.h"
#include "EndgameAsset.h"
#include "core/SettingsManager.h"
#include "Effects.h"
#include "EntityFactory.h"
#include "EnemyFactory.h"
#include "GameState.h"
#include "ItemAtlasRegistry.h"
#include "infrastructure/AssetManager.h"
#include "infrastructure/AudioManager.h"
#include "ItemFactory.h"
#include "PlayerCommands.h"
#include "PlayerFactory.h"
#include "SaveManager.h"
#include "WorldCatalog.h"
#include "Flag.h"
#include "ShopAsset.h"
#include "PlayerHUD.h"
#include "Coin.h"
#include "CustomMapData.h"
#include "DialogueRegistry.h"
#include <algorithm>
#include <iostream>
#include <raylib.h>
#include <raymath.h>


BaseLevelState::BaseLevelState(const std::string &mapFilePath,
                               const std::string &initialLevel,
                               const std::string &p1Name,
                               const std::string &p2Name,
                               bool isPvPMode)
    : mapCamera(600.0f), currentLevel(initialLevel), mapFilePath(mapFilePath), isPvPMode_(isPvPMode) {

  std::cout << "[BaseLevelState] Loading map: " << mapFilePath << " level: " << initialLevel << "\n";
  if (map.LoadLDtkMap(mapFilePath, initialLevel)) {
    std::cout << "[BaseLevelState] Map loaded successfully!\n";
    
    std::string bgSound = map.GetBackgroundSound();
    if (!bgSound.empty()) {
        AudioManager::getInstance().PlayBackgroundSound(bgSound);
    } else {
        // Dừng nhạc Menu nếu map không có nhạc nền
        AudioManager::getInstance().StopAll(); 
    }

    auto spawns = map.GetPlayerSpawns();
    Vector2 spawn1 = spawns.size() > 0 ? spawns[0] : Vector2{180.0f, 150.0f};

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

    if (!p2Name.empty()) {
      player2 = PlayerFactory::createPlayer(p2Name, {0, 0});
      if (player2) {
        Vector2 spawn2 = spawns.size() > 1 ? spawns[1] : Vector2{220.0f, 150.0f};
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
    }
    
    if (player1 && player2 && isPvPMode_) {
        player1->setPvPMode(true);
        player2->setPvPMode(true);
    }
    
    bindPlayerInputs();

    ItemAtlasRegistry::getInstance().loadAll("assets/maps/item/");
    ItemAtlasRegistry::getInstance().loadAtlas("mob_mushroom", "assets/mobs/mob_mushroom.json", "assets/mobs/mob_mushroom.png");
    ItemAtlasRegistry::getInstance().loadAtlas("mob_rat", "assets/mobs/mob_rat.json", "assets/mobs/mob_rat.png");
    ItemAtlasRegistry::getInstance().loadAtlas("mob_tree", "assets/mobs/mob_tree.json", "assets/mobs/mob_tree.png");
    ItemAtlasRegistry::getInstance().loadAtlas("mob_skeleton", "assets/mobs/mob_skeleton.json", "assets/mobs/mob_skeleton.png");
    ItemAtlasRegistry::getInstance().loadAtlas("mob_goblin", "assets/mobs/mob_goblin.json", "assets/mobs/mob_goblin.png");
    ItemAtlasRegistry::getInstance().loadAtlas("mob_guardian", "assets/mobs/mob_guardian.json", "assets/mobs/mob_guardian.png");
    ItemAtlasRegistry::getInstance().loadAtlas("mob_bat", "assets/mobs/mob_bat.json", "assets/mobs/mob_bat.png");
    ItemAtlasRegistry::getInstance().loadAtlas("mob_soldier", "assets/mobs/mob_soldier.json", "assets/mobs/mob_soldier.png");
    ItemAtlasRegistry::getInstance().loadAtlas("mob_slime", "assets/mobs/mob_slime.json", "assets/mobs/mob_slime.png");
    AssetManager::getInstance().loadTexture("arrow_soldier", "assets/mobs/arrow_soldier.png");

    spawnEntitiesFromMap();
    spawnCutsceneTriggersFromMap();
    TraceLog(LOG_INFO, "[BaseLevelState] Spawned %d items and %d entities.", activeItems.size(), activeEntities.size());
  } else {
    std::cerr << "[BaseLevelState] Error loading " << mapFilePath << "!\n";
  }

  // Initialize In-Game Settings Panel
  ingameSettings_ = std::make_unique<IngameSettingsPanel>();
  ingameSettings_->init((float)GetScreenWidth(), (float)GetScreenHeight(), [this]() {
      this->PushStateCommand(std::make_unique<ChangeStateCommand>(std::make_unique<MainMenuState>()));
  });

  // Initialize Shop UI
  shopUI_ = std::make_unique<ShopUIPanel>();
  shopUI_->init((float)GetScreenWidth(), (float)GetScreenHeight());
}

// =============================================================================
// [NEW] Constructor khôi phục từ bản lưu (luồng LOAD GAME).
//
// Dùng DELEGATING CONSTRUCTOR (C++11): thay vì chép lại toàn bộ 100 dòng khởi
// tạo map/player/input/UI, ta gọi thẳng constructor LDtk ở trên với hai thông
// tin rút ra từ save:
//     - level nào  -> save.levelData.levelId
//     - nhân vật nào -> save.p1.characterName
// Constructor gốc chạy xong sẽ có một màn chơi "sạch" đúng level đó; phần thân
// dưới đây mới phủ lên trạng thái đã lưu (máu, coin, quái đã chết, item đã mở).
//
// Fallback "Goku" phòng khi đọc phải bản lưu đời cũ chưa có characterName —
// còn hơn là dựng Player rỗng rồi crash.
// =============================================================================
BaseLevelState::BaseLevelState(const std::string &mapFilePath, const GameSaveData &save)
    : BaseLevelState(mapFilePath,
                     save.levelData.levelId,
                     save.p1.characterName.empty() ? std::string("Goku")
                                                   : save.p1.characterName) {
  restoreFromSaveData(save);

  // Khôi phục xong thì bản lưu này cũng chính là checkpoint hiện hành: chết
  // trong màn sẽ hồi sinh về đúng đây thay vì về đầu map.
  SaveManager::getInstance().setCheckpoint(save);
}

// =============================================================================
// [NEW] Constructor cho chế độ Test Play từ MapEditorState
// =============================================================================
BaseLevelState::BaseLevelState(const CustomMapData& customMap,
                               const std::string &p1Name,
                               const std::string &p2Name)
    : mapCamera(600.0f), currentLevel(customMap.name), mapFilePath("custom") {

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
    
    if (player1 && player2) {
        player1->setPvPMode(true);
        player2->setPvPMode(true);
        isPvPMode_ = true;
    }

    ItemAtlasRegistry::getInstance().loadAll("assets/maps/item/");
    ItemAtlasRegistry::getInstance().loadAtlas("mob_mushroom", "assets/mobs/mob_mushroom.json", "assets/mobs/mob_mushroom.png");
    ItemAtlasRegistry::getInstance().loadAtlas("mob_rat", "assets/mobs/mob_rat.json", "assets/mobs/mob_rat.png");
    ItemAtlasRegistry::getInstance().loadAtlas("mob_tree", "assets/mobs/mob_tree.json", "assets/mobs/mob_tree.png");
    ItemAtlasRegistry::getInstance().loadAtlas("mob_skeleton", "assets/mobs/mob_skeleton.json", "assets/mobs/mob_skeleton.png");
    ItemAtlasRegistry::getInstance().loadAtlas("mob_goblin", "assets/mobs/mob_goblin.json", "assets/mobs/mob_goblin.png");
    ItemAtlasRegistry::getInstance().loadAtlas("mob_guardian", "assets/mobs/mob_guardian.json", "assets/mobs/mob_guardian.png");
    ItemAtlasRegistry::getInstance().loadAtlas("mob_bat", "assets/mobs/mob_bat.json", "assets/mobs/mob_bat.png");
    ItemAtlasRegistry::getInstance().loadAtlas("mob_soldier", "assets/mobs/mob_soldier.json", "assets/mobs/mob_soldier.png");
    ItemAtlasRegistry::getInstance().loadAtlas("mob_slime", "assets/mobs/mob_slime.json", "assets/mobs/mob_slime.png");
    AssetManager::getInstance().loadTexture("arrow_soldier", "assets/mobs/arrow_soldier.png");

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

  // Initialize In-Game Settings Panel for Custom Map
  ingameSettings_ = std::make_unique<IngameSettingsPanel>();
  ingameSettings_->init((float)GetScreenWidth(), (float)GetScreenHeight(), [this]() {
      this->PushStateCommand(std::make_unique<ChangeStateCommand>(std::make_unique<MainMenuState>()));
  });

  // Initialize Shop UI
  shopUI_ = std::make_unique<ShopUIPanel>();
  shopUI_->init((float)GetScreenWidth(), (float)GetScreenHeight());
}


void BaseLevelState::HandleInput() {
  // In-Game Settings input check
  if (enableIngameSettings_ && ingameSettings_) {
    Vector2 mousePos = GetMousePosition();
    bool mousePressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    bool mouseReleased = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
    if (ingameSettings_->handleInput(mousePos, mousePressed, mouseReleased)) {
      if (ingameSettings_->isOpen()) {
        if (player1 && player1->getRuntimeStats().isGrounded) {
          player1->stopLeftRun();
          player1->stopRightRun();
        }
        if (player2 && player2->getRuntimeStats().isGrounded) {
          player2->stopLeftRun();
          player2->stopRightRun();
        }
        return; // Pause player & AI input when settings is open
      }
    }
  }

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
    if (player2) {
      auto commands = player2Handler.handleInput();
      for (auto *cmd : commands)
        cmd->Execute(*player2);
    }
  }

  // Entity AI vẫn chạy bình thường kể cả khi cutscene đang diễn ra
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

  if (enableIngameSettings_ && ingameSettings_) {
    ingameSettings_->process();
  }

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
      if (player2) player2->onCutsceneEnd(finishedId);
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
  if (player2) checkEdge(player2.get());

  if (!next.empty()) {
    TransitionToLevel(next, dir, tX, tY);
  }

  if (!cutsceneManager.isActive()) {
    if (player1 && player2) {
      Vector2 p1Pos = player1->getWorldStats().position;
      Vector2 p2Pos = player2->getWorldStats().position;
      Vector2 avgVelocity = {
          (player1->getRuntimeStats().velocity.x + player2->getRuntimeStats().velocity.x) / 2.0f,
          (player1->getRuntimeStats().velocity.y + player2->getRuntimeStats().velocity.y) / 2.0f
      };
      mapCamera.Update(p1Pos, avgVelocity, map.GetWidth(), map.GetHeight(), dt, &p2Pos);
    } else if (player1) {
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
  if (enableIngameSettings_ && ingameSettings_) {
    ingameSettings_->update(dt);
    if (ingameSettings_->isOpen()) {
      return; // Pause game world updates while settings is open
    }
  }

  if (shopUI_ && shopUI_->isOpen()) {
    shopUI_->update(dt, GetMousePosition(), IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
    return; // Đóng băng logic game khi shop mở
  }

  // Đếm giờ chơi. Đặt SAU hai lần return ở trên để thời gian đứng trong menu
  // Settings và trong Shop không bị tính — con số hiển thị ở panel LOAD GAME
  // phản ánh đúng thời gian chơi thật.
  playTimeSeconds_ += dt;

  // Quét các activeItems xem có ShopAsset nào yêu cầu mở shop không, hoặc EndgameAsset nào được chạm chưa
  for (auto& item : activeItems) {
      if (item && item->getIsActive()) {
          ShopAsset* shop = dynamic_cast<ShopAsset*>(item.get());
          if (shop && shop->wantsToOpenShop()) {
              shopUI_->open(player1.get());
              shop->resetOpenShop();
          }

          EndgameAsset* endgame = dynamic_cast<EndgameAsset*>(item.get());
          if (endgame && endgame->isReached_) {
              this->PushStateCommand(std::make_unique<ChangeStateCommand>(std::make_unique<EndgameState>(isPvPMode_)));
              return;
          }
      }
  }

  // Update cutscene mode if active
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

  if (player2) {
    player2->updatePhysicsWithMap(map, dynamicSolids, dt);
    player2->updateStateFromPhysics();
    player2->update(dt);
  }
  bool timeStopped = false;
  if (player1 && player1->getBuffManager().canTimeStop()) timeStopped = true;
  if (player2 && player2->getBuffManager().canTimeStop()) timeStopped = true;

  for (auto &entity : activeEntities) {
    if (timeStopped && entity->getFaction() == EntityFaction::Enemy) continue;
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
    Rectangle itemBox = item->getHitbox();
    auto handleInteract = [&](Player* p) {
        if (p && CheckCollisionRecs(itemBox, p->getHitbox())) {
            ItemState oldState = item->getItemState();
            item->onInteract(*p);
            
            // Check if it's a flag that just got activated
            if (oldState != ItemState::Active && item->getItemState() == ItemState::Active && dynamic_cast<Flag*>(item.get()) != nullptr) {
                onCheckpointReached();
            }
        }
    };
    
    handleInteract(player1.get());
    handleInteract(player2.get());
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

  for (const auto& trigger : cutsceneTriggers) {
      trigger.renderDebug();
  }
  
  mapCamera.EndMode();

  PlayerHUD::render(player1.get(), player2.get(), partyInventory.get());

  // Cutscene dialogue box vẽ trên cùng (screen space, ngoài camera)
  if (cutsceneManager.isActive()) {
    cutsceneManager.render(alpha);
  }

  // In-Game Settings overlay (cogwheel button or full Settings Panel)
  if (enableIngameSettings_ && ingameSettings_) {
    ingameSettings_->render(alpha);
  }

  // Shop UI overlay
  if (shopUI_ && shopUI_->isOpen()) {
    shopUI_->render(alpha);
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
    
    std::string bgSound = map.GetBackgroundSound();
    if (!bgSound.empty()) {
        AudioManager::getInstance().PlayBackgroundSound(bgSound);
    } else {
        AudioManager::getInstance().StopAll();
    }
    
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

// =============================================================================
// SAVE / LOAD
// =============================================================================

GameSaveData BaseLevelState::createSaveData() const {
  GameSaveData data;
  data.isValid = true;

  // --- Trạng thái map: chỉ ghi phần SAI KHÁC so với file .ldtk gốc ---------
  int worldIndex = WorldCatalog::getInstance().indexFromMapPath(mapFilePath);
  data.levelData.worldIndex = worldIndex;
  data.levelData.worldId = worldIndex > 0
                               ? ("world0" + std::to_string(worldIndex))
                               : std::string("");
  data.levelData.levelId = currentLevel;
  data.levelData.mapFilePath = mapFilePath;
  data.levelData.persistedItemStates = persistedItemStates;
  data.levelData.persistedDeadEntities = persistedDeadEntities;

  // Ảnh chụp trạng thái item PHẢI lấy từ các item đang sống trên màn, không chỉ
  // từ map persisted: những item vừa bị đổi trạng thái ở level hiện tại chưa
  // được đẩy vào persistedItemStates (việc đó chỉ xảy ra lúc TransitionToLevel).
  for (const auto& item : activeItems) {
    if (item && !item->getIid().empty()) {
      data.levelData.persistedItemStates[item->getIid()] = item->getItemState();
    }
  }

  if (partyInventory) {
    data.inventory.coins = partyInventory->coins;
    data.inventory.keys = partyInventory->keys;
  }

  // --- Trạng thái người chơi: uỷ quyền, không thò tay vào nội bộ Player ----
  if (player1) {
    data.p1 = player1->createSaveData();
  }

  // --- Bìa sách cho panel LOAD GAME ---------------------------------------
  // (versionIndex + thời điểm lưu do FileSaveRepository điền nốt khi ghi file)
  data.meta.worldIndex      = worldIndex;
  data.meta.levelId         = currentLevel;
  data.meta.characterName   = data.p1.characterName;
  data.meta.playTimeSeconds = playTimeSeconds_;
  data.meta.coins           = data.inventory.coins;
  data.meta.health          = data.p1.health;
  data.meta.maxHealth       = data.p1.maxHealth;

  return data;
}

void BaseLevelState::restoreFromSaveData(const GameSaveData& data) {
  if (!data.isValid) return;

  if (partyInventory) {
    partyInventory->coins = data.inventory.coins;
    partyInventory->keys = data.inventory.keys;
  }

  if (player1) {
    player1->restoreFromSaveData(data.p1);
  }

  persistedItemStates = data.levelData.persistedItemStates;
  persistedDeadEntities = data.levelData.persistedDeadEntities;
  playTimeSeconds_ = data.meta.playTimeSeconds;

  activeEntities.clear();
  activeItems.clear();
  combatSystem = CombatSystem();

  map.LoadLDtkMap(data.levelData.mapFilePath, data.levelData.levelId);
  currentLevel = data.levelData.levelId;

  spawnEntitiesFromMap();
  // Trước đây thiếu dòng này: sau mỗi lần hồi sinh từ checkpoint, toàn bộ vùng
  // kích hoạt cutscene của level biến mất vì chỉ entity/item được spawn lại.
  spawnCutsceneTriggersFromMap();
}

// -----------------------------------------------------------------------------
// Chạm Flag = một mốc tiến trình -> vừa đặt checkpoint trong RAM (để hồi sinh),
// vừa ghi HẲN một bản lưu mới xuống saves/world0X/versionN.json.
//
// HAI LỚP BẢO VỆ để tính năng này không rò sang chế độ khác:
//   1. worldIndex < 0  : đường dẫn map không thuộc 6 world 1-player nào —
//                        loại luôn menu.ldtk, pvp_map0N/ và custom map.
//   2. isPvPMode_ || player2 : chắc chắn đang ở chế độ một người chơi.
// Nhờ WorldCatalog trả lời câu hỏi số 1, ta không phải rải if/else kiểm tra
// chế độ khắp BaseLevelState.
// -----------------------------------------------------------------------------
void BaseLevelState::onCheckpointReached() {
  int worldIndex = WorldCatalog::getInstance().indexFromMapPath(mapFilePath);
  if (worldIndex < 0) return;
  if (isPvPMode_ || player2) return;

  GameSaveData data = createSaveData();

  // Checkpoint trong RAM: giữ nguyên hành vi hồi sinh cũ.
  SaveManager::getInstance().setCheckpoint(data);

  // Bản lưu trên đĩa: mỗi lần chạm Flag sinh thêm một version mới.
  SaveSlotInfo created;
  if (SaveManager::getInstance().createVersion(worldIndex, data, created)) {
    std::cout << "[BaseLevelState] Checkpoint -> " << created.filePath << "\n";
  }
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
  // === Chế độ đối kháng: ai gục trước thì người còn lại thắng ===============
  // Khác hẳn 1-Player: KHÔNG hồi sinh, ván đấu kết thúc ngay và chuyển sang
  // màn hình trao giải kèm tên nhân vật thắng để EndgameState dựng hoạt ảnh.
  // Đặt trước toàn bộ logic respawn bên dưới vì hai luồng loại trừ nhau.
  if (isPvPMode_ && player1 && player2) {
    // Đã có người gục -> đang đếm ngược, chỉ chờ hết giờ rồi chuyển màn.
    if (pvpEndTimer_ > 0.0f) {
      pvpEndTimer_ -= dt;
      if (pvpEndTimer_ <= 0.0f) {
        pvpEndTimer_ = -1.0f;
        this->PushStateCommand(std::make_unique<ChangeStateCommand>(
            std::make_unique<EndgameState>(true, pvpWinnerName_)));
      }
      return;
    }

    auto isDown = [&](Player *p) {
      return p && (p->isDead() || p->isOutOfBounds(map.GetHeight() + 32.0f));
    };

    bool p1Down = isDown(player1.get());
    bool p2Down = isDown(player2.get());

    if (p1Down || p2Down) {
      if (p1Down && p2Down) {
        pvpWinnerName_ = "";   // cùng gục trong một frame -> hoà
      } else if (p1Down) {
        pvpWinnerName_ = player2->getBaseStats().name;
      } else {
        pvpWinnerName_ = player1->getBaseStats().name;
      }

      // Rơi ra ngoài map nhưng máu vẫn còn -> kết liễu để animation chết chạy,
      // giống hệt cách luồng 1-Player xử lý.
      if (p1Down && !player1->isDead()) player1->takeDamage(9999);
      if (p2Down && !player2->isDead()) player2->takeDamage(9999);

      std::cout << "[BaseLevelState] PvP ket thuc. Nguoi thang: "
                << (pvpWinnerName_.empty() ? "HOA" : pvpWinnerName_) << "\n";

      pvpEndTimer_ = 1.5f;   // chờ animation chết rồi mới sang màn trao giải
    }
    return;
  }

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
                onCheckpointReached();
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
        cutsceneManager.startCutscene(trigger.getScript(), mapCamera, playerPos, map.GetWidth(), map.GetHeight());
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
