# MEMBER CONTRIBUTION — GROUP 51

**Course:** CS202 – Programming Systems · **Class:** 25A01
**Project:** Super Mario Plus · **Period:** 8 June 2026 → 1 September 2026

| Student ID | Full name | Commits |
|---|---|---|
| 25125028 | Phạm Đức Minh | 30 |
| 25125007 | Lê Tiến Bình | 38 |
| | **Total** | **68** |

Every date below is the actual commit date, verifiable with:

```bash
git log --all --format="%ad | %an | %s" --date=short
```

> **Hours column left blank.** The template asks for hours per task. We have not filled these in
> because only you know how long each task actually took — enter your own figures when pasting.

---

## Rows to paste into `member_contribution.xlsx`

Tasks are divided so that no task is duplicated between members, following the template's note that
*"a task should be done by only 1–2 students"*.

| No | Student ID | Full name | Task Description | Hours | Date |
|---|---|---|---|---|---|
| 1 | 25125007 | Lê Tiến Bình | Set up the project skeleton, CMake build configuration and vendored dependencies | | 2026-06-08 |
| 2 | 25125007 | Lê Tiến Bình | Define the four-phase frame structure and the project's coding rules | | 2026-06-09 |
| 3 | 25125028 | Phạm Đức Minh | Design the overall screen flow and the game's state architecture | | 2026-06-12 |
| 4 | 25125028 | Phạm Đức Minh | Implement the screen stack and screen transition mechanism | | 2026-06-13 |
| 5 | 25125007 | Lê Tiến Bình | Restructure the source tree into modules and repair the asset and font set | | 2026-06-14 |
| 6 | 25125028 | Phạm Đức Minh | Build the player state machine with ten behaviour classes | | 2026-07-14 |
| 7 | 25125028 | Phạm Đức Minh | Build the animation system and wire attack animations to character data | | 2026-07-20 |
| 8 | 25125028 | Phạm Đức Minh | Add a second playable character and make character definitions data-driven | | 2026-07-22 |
| 9 | 25125007 | Lê Tiến Bình | Implement the LDtk level parser: tilesets, tile layers, collision grid and entity instances | | 2026-07-27 |
| 10 | 25125007 | Lê Tiến Bình | Implement tile physics for worlds 1 and 2, including one-way platforms and ladders | | 2026-07-28 |
| 11 | 25125007 | Lê Tiến Bình | Author world 3 and extend terrain handling to water, lava, poison and cloud tiles | | 2026-07-31 |
| 12 | 25125028 | Phạm Đức Minh | Implement the skill system and the four-hit melee combo chain | | 2026-08-01 |
| 13 | 25125028 | Phạm Đức Minh | Implement projectiles with configurable speed, damage, lifetime and curved trajectories | | 2026-08-06 |
| 14 | 25125007 | Lê Tiến Bình | Author and correct the tile work for all six worlds | | 2026-08-07 |
| 15 | 25125007 | Lê Tiến Bình | Merge the map/camera branch with the combat branch, unify the character data structure, and add the swimming and climbing behaviours | | 2026-08-07 |
| 16 | 25125007 | Lê Tiến Bình | Complete the branch integration and restore all levels to a playable state | | 2026-08-08 |
| 17 | 25125028 | Phạm Đức Minh | Plan the enemy architecture and integrate it with the merged codebase | | 2026-08-10 |
| 18 | 25125007 | Lê Tiến Bình | Revise level layouts across the campaign | | 2026-08-15 |
| 19 | 25125028 | Phạm Đức Minh | Import and organise the enemy and boss sprite assets | | 2026-08-17 |
| 20 | 25125028 | Phạm Đức Minh | Fix the combat system so damage is only applied during each attack's active frames | | 2026-08-18 |
| 21 | 25125028 | Phạm Đức Minh | Add health and attack animation feedback, and debug skill execution timing | | 2026-08-18 |
| 22 | 25125007 | Lê Tiến Bình | Implement the item system: coins, keys, doors, chests, blocks, springs and the one-slot inventory | | 2026-08-18 |
| 23 | 25125007 | Lê Tiến Bình | Resolve the second branch merge, including two conflicting combat implementations, and fix the resulting build errors | | 2026-08-19 |
| 24 | 25125028 | Phạm Đức Minh | Implement nine enemy types with a seven-state AI: patrol, detection, chase, attack, hurt and death | | 2026-08-22 |
| 25 | 25125028 | Phạm Đức Minh | Tune enemy behaviour and add ledge and hazard avoidance | | 2026-08-23 |
| 26 | 25125028 | Phạm Đức Minh | Implement the cutscene system with queued camera pan and zoom behaviours | | 2026-08-27 |
| 27 | 25125028 | Phạm Đức Minh | Fix the camera so an interrupted cutscene restores normal following | | 2026-08-27 |
| 28 | 25125007 | Lê Tiến Bình | Build the main menu, the settings screen and the key rebinding interface for both players | | 2026-08-29 |
| 29 | 25125007 | Lê Tiến Bình | Build the in-game map editor: block palette, entity palette, tools and auto-tiling | | 2026-08-29 |
| 30 | 25125028 | Phạm Đức Minh | Implement six boss types with a nine-state AI and a scripted introduction | | 2026-08-29 |
| 31 | 25125028 | Phạm Đức Minh | Fix boss attack areas by defining them per attack instead of deriving them from sprite size | | 2026-08-29 |
| 32 | 25125028 | Phạm Đức Minh | Add jump decision-making to enemy AI and a dash multiplier to enemy skills | | 2026-08-29 |
| 33 | 25125007 | Lê Tiến Bình | Build the world selection and character selection screens with live character preview | | 2026-08-30 |
| 34 | 25125028 | Phạm Đức Minh | Complete the full set of enemy types and their configuration | | 2026-08-30 |
| 35 | 25125007 | Lê Tiến Bình | Debug the character selection screen and fix same-frame input consumption | | 2026-08-30 |
| 36 | 25125007 | Lê Tiến Bình | Build the shop interface with a scrollable catalogue and coin-based purchasing | | 2026-08-31 |
| 37 | 25125028 | Phạm Đức Minh | Tune boss AI behaviour after playtesting | | 2026-08-31 |
| 38 | 25125028 | Phạm Đức Minh | Correct level design issues in world 1 | | 2026-08-31 |
| 39 | 25125007 | Lê Tiến Bình | Implement the multi-version save system with a four-layer architecture and atomic file writes | | 2026-09-01 |
| 40 | 25125007 | Lê Tiến Bình | Implement the endgame screen for both story completion and versus victory | | 2026-09-01 |
| 41 | 25125007 | Lê Tiến Bình | Add animated GIF support and resolution-independent interface scaling | | 2026-09-01 |
| 42 | 25125007 | Lê Tiến Bình | Fix outstanding project defects and merge the final branch into main | | 2026-09-01 |

---

## Task distribution

| Member | Tasks | Commits | Main areas |
|---|---|---|---|
| **Phạm Đức Minh** (25125028) | 18 | 30 | Player state machine, animation, skills and combos, combat resolution, projectiles, nine enemy types, six bosses, enemy AI, cutscenes and camera behaviours |
| **Lê Tiến Bình** (25125007) | 24 | 38 | Build setup, LDtk level loading, tile physics and terrain, level authoring, items and inventory, all user interface, map editor, shop, save system, endgame, branch integration |
| **Total** | **42** | **68** | |

---

## How to fill the spreadsheet

1. Open `Group51/member_contribution.xlsx`.
2. On the contribution sheet, paste the rows above into the columns
   `No · Student ID · Full name · Task Description · Hours · Evidence`, leaving `Evidence` empty.
3. Enter your own figure in the `Hours` column for each row.
4. Leave the grey cells alone — the percentage and score columns are computed, and the teaching
   assistants fill in the score.

The summary block at the top of the sheet asks for the number of students (**2**), the number of tasks
(**42**) and the number of Git commits (**68**).

---

*Group 51 — Phạm Đức Minh (25125028) · Lê Tiến Bình (25125007)*
