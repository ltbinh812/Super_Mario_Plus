import json
import re

mobs_json = """{
  "mob_slime": {
    "name": "mob_slime",
    "maxHealth": 45,
    "moveVelocity": 50.0,
    "jumpVelocity": 150.0,
    "gravityScale": 160.0,
    "physicsBox": {
      "w": 30.0,
      "h": 30.0
    },
    "aiData": {
      "detectionRange": 200.0,
      "attackRange": 35.0,
      "patrolSpeed": 30.0,
      "patrolTime": 3.0
    },
    "animationFrames": {
      "attack": 8,
      "run": 8,
      "idle": 6,
      "hurt": 4,
      "die": 10
    },
    "enemySkills": {
      "mob_slime_attack": {
        "damage": 5,
        "hitboxStartFrame": 3,
        "hitboxEndFrame": 6,
        "box": {
          "offsetX": 10.0,
          "offsetY": -5.0,
          "w": 35.0,
          "h": 35.0
        },
        "dashMultiplier": 2.0
      }
    }
  },
  "mob_soldier": {
    "name": "mob_soldier",
    "maxHealth": 80,
    "moveVelocity": 90.0,
    "jumpVelocity": 200.0,
    "gravityScale": 160.0,
    "physicsBox": {
      "w": 32.0,
      "h": 45.0
    },
    "aiData": {
      "detectionRange": 350.0,
      "attackRange": 150.0,
      "patrolSpeed": 50.0,
      "patrolTime": 4.0
    },
    "animationFrames": {
      "attack": 6,
      "run": 8,
      "idle": 6,
      "projectile": 6,
      "hurt": 4,
      "die": 4
    },
    "enemySkills": {
      "mob_soldier_attack": {
        "damage": 10,
        "hitboxStartFrame": 3,
        "hitboxEndFrame": 5,
        "box": {
          "offsetX": 10.0,
          "offsetY": -10.0,
          "w": 30.0,
          "h": 45.0
        },
        "dashMultiplier": 1.5
      },
      "mob_soldier_projectile": {
        "damage": 15,
        "hitboxStartFrame": 4,
        "hitboxEndFrame": 5,
        "box": {
          "offsetX": 20.0,
          "offsetY": -15.0,
          "w": 10.0,
          "h": 10.0
        },
        "dashMultiplier": 0.0
      }
    },
    "fireball": {
      "damage": 15,
      "textureName": "arrow_soldier",
      "frameNum": 1,
      "frameTime": 0.1,
      "hitboxW": 32.0,
      "hitboxH": 32.0,
      "offsetX": 10.0,
      "offsetY": -20.0,
      "speed": 400.0,
      "lifetime": 2.0
    }
  },
  "mob_bat": {
    "name": "mob_bat",
    "maxHealth": 35,
    "moveVelocity": 150.0,
    "jumpVelocity": 0.0,
    "gravityScale": 50.0,
    "physicsBox": {
      "w": 30.0,
      "h": 30.0
    },
    "aiData": {
      "detectionRange": 350.0,
      "attackRange": 50.0,
      "patrolSpeed": 80.0,
      "patrolTime": 4.0
    },
    "animationFrames": {
      "attack": 7,
      "run": 6,
      "idle": 15,
      "hurt": 5,
      "die": 11
    },
    "enemySkills": {
      "mob_bat_attack": {
        "damage": 5,
        "hitboxStartFrame": 3,
        "hitboxEndFrame": 5,
        "box": {
          "offsetX": 10.0,
          "offsetY": -10.0,
          "w": 40.0,
          "h": 40.0
        },
        "dashMultiplier": 2.5
      }
    }
  },
  "mob_guardian": {
    "name": "mob_guardian",
    "maxHealth": 200,
    "moveVelocity": 70.0,
    "jumpVelocity": 200.0,
    "gravityScale": 200.0,
    "physicsBox": {
      "w": 50.0,
      "h": 80.0
    },
    "aiData": {
      "detectionRange": 400.0,
      "attackRange": 60.0,
      "patrolSpeed": 40.0,
      "patrolTime": 5.0
    },
    "animationFrames": {
      "attack": 14,
      "run": 10,
      "idle": 6,
      "hurt": 7,
      "die": 16
    },
    "enemySkills": {
      "mob_guardian_attack": {
        "damage": 25,
        "hitboxStartFrame": 6,
        "hitboxEndFrame": 10,
        "box": {
          "offsetX": 25.0,
          "offsetY": -10.0,
          "w": 60.0,
          "h": 80.0
        },
        "dashMultiplier": 1.5
      }
    }
  },
  "mob_goblin": {
    "name": "mob_goblin",
    "maxHealth": 40,
    "moveVelocity": 110.0,
    "jumpVelocity": 280.0,
    "gravityScale": 160.0,
    "physicsBox": {
      "w": 28.0,
      "h": 40.0
    },
    "aiData": {
      "detectionRange": 300.0,
      "attackRange": 40.0,
      "patrolSpeed": 60.0,
      "patrolTime": 3.0
    },
    "animationFrames": {
      "attack": 8,
      "run": 8,
      "idle": 4,
      "hurt": 4,
      "die": 4
    },
    "enemySkills": {
      "mob_goblin_attack": {
        "damage": 8,
        "hitboxStartFrame": 3,
        "hitboxEndFrame": 6,
        "box": {
          "offsetX": 10.0,
          "offsetY": -10.0,
          "w": 30.0,
          "h": 40.0
        },
        "dashMultiplier": 2.0
      }
    }
  },
  "mob_skeleton": {
    "name": "mob_skeleton",
    "maxHealth": 60,
    "moveVelocity": 90.0,
    "jumpVelocity": 250.0,
    "gravityScale": 160.0,
    "physicsBox": {
      "w": 32.0,
      "h": 50.0
    },
    "aiData": {
      "detectionRange": 250.0,
      "attackRange": 45.0,
      "patrolSpeed": 50.0,
      "patrolTime": 3.0
    },
    "animationFrames": {
      "attack": 8,
      "run": 4,
      "idle": 4,
      "hurt": 4,
      "die": 4
    },
    "enemySkills": {
      "mob_skeleton_attack": {
        "damage": 10,
        "hitboxStartFrame": 3,
        "hitboxEndFrame": 6,
        "box": {
          "offsetX": 15.0,
          "offsetY": -10.0,
          "w": 40.0,
          "h": 50.0
        },
        "dashMultiplier": 1.5
      }
    }
  },
  "mob_tree": {
    "name": "mob_tree",
    "maxHealth": 100,
    "moveVelocity": 60.0,
    "jumpVelocity": 200.0,
    "gravityScale": 180.0,
    "physicsBox": {
      "w": 40.0,
      "h": 60.0
    },
    "aiData": {
      "detectionRange": 200.0,
      "attackRange": 50.0,
      "patrolSpeed": 40.0,
      "patrolTime": 4.0
    },
    "animationFrames": {
      "attack": 8,
      "run": 8,
      "idle": 4,
      "hurt": 4,
      "die": 4
    },
    "enemySkills": {
      "mob_tree_attack": {
        "damage": 15,
        "hitboxStartFrame": 3,
        "hitboxEndFrame": 6,
        "box": {
          "offsetX": 20.0,
          "offsetY": -10.0,
          "w": 40.0,
          "h": 60.0
        },
        "dashMultiplier": 1.0
      }
    }
  },
  "mob_rat": {
    "name": "mob_rat",
    "maxHealth": 30,
    "moveVelocity": 120.0,
    "jumpVelocity": 300.0,
    "gravityScale": 160.0,
    "physicsBox": {
      "w": 24.0,
      "h": 24.0
    },
    "aiData": {
      "detectionRange": 300.0,
      "attackRange": 35.0,
      "patrolSpeed": 80.0,
      "patrolTime": 3.0
    },
    "animationFrames": {
      "attack": 6,
      "run": 6,
      "idle": 6,
      "hurt": 1,
      "die": 6
    },
    "enemySkills": {
      "mob_rat_attack": {
        "damage": 5,
        "hitboxStartFrame": 2,
        "hitboxEndFrame": 4,
        "box": {
          "offsetX": 10.0,
          "offsetY": -5.0,
          "w": 32.0,
          "h": 32.0
        },
        "dashMultiplier": 2.0
      }
    }
  }
}"""

with open('d:/super_mario/Super_Mario_Plus/assets/config/enemies.json', 'r', encoding='utf-8') as f:
    d = json.load(f)

# Insert the lost mobs
mobs = json.loads(mobs_json)
for k, v in mobs.items():
    d[k] = v

# Fix boss attack boxes according to user's changes
bosses = ['boss_doflam', 'boss_franky', 'boss_itachi', 'boss_sasukeboss', 'boss_shank', 'boss_narutoboss']
boss_boxes = {
    'attack_1': {'offsetX': 20.0, 'offsetY': -10.0, 'w': 50.0, 'h': 60.0},
    'attack_2': {'offsetX': 30.0, 'offsetY': -15.0, 'w': 60.0, 'h': 70.0},
    'attack_3': {'offsetX': 10.0, 'offsetY': -20.0, 'w': 40.0, 'h': 50.0},
    'attack_4': {'offsetX': 25.0, 'offsetY': -5.0, 'w': 55.0, 'h': 65.0}
}

for boss in bosses:
    if boss in d and 'enemySkills' in d[boss]:
        for atk, b in boss_boxes.items():
            if atk in d[boss]['enemySkills']:
                d[boss]['enemySkills'][atk]['box'] = dict(b)

# Save the file with formatting
content = json.dumps(d, indent=2)

# Apply the -30 offset fix requested previously by the user
def repl(match):
    prefix = match.group(1)
    val = float(match.group(2))
    if val > -30:
        val = -30.0
    if match.group(2).isdigit() or (match.group(2).startswith('-') and match.group(2)[1:].isdigit()):
        return f'{prefix}{int(val)}'
    else:
        return f'{prefix}{val}'

content = re.sub(r'("offsetY"\s*:\s*)(-?\d+(?:\.\d+)?)', repl, content)

with open('d:/super_mario/Super_Mario_Plus/assets/config/enemies.json', 'w', encoding='utf-8') as f:
    f.write(content)
print("Restored mobs and applied offset fix.")
