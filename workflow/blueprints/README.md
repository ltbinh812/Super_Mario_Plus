# Architecture Blueprints

PlantUML diagrams describing the **current** structure of Super Mario Plus.

**Verified against commit `b378acb`** (1 September 2026). Every class, interface and inheritance edge
in these files was checked to exist in `include/` or `src/` at that commit. If you change the
architecture, update the affected file here as well.

## The set

| File | What it shows |
|---|---|
| `core_engine.puml` | The game loop, the screen stack, and the four-phase frame rule |
| `entities_architecture.puml` | The `Entity` hierarchy and who owns what |
| `player_state_machine.puml` | The player's ten behaviours and every transition between them |
| `enemy_ai_state_machine.puml` | The seven enemy behaviours and the nine boss behaviours |
| `behaviour_interfaces.puml` | Skills, power-ups and item usage — including why power-ups are deliberately *not* Strategy |
| `command_pattern.puml` | The two independent Command pipelines: screen transitions and player input |
| `world_and_collision.puml` | Level loading, the thirteen terrain types, camera behaviours and combat resolution |
| `save_architecture.puml` | The four-layer persistence stack with its injected dependencies |
| `map_editor.puml` | Editor tools, snapshot-based undo, auto-tiling, and the bridge into gameplay |
| `ui_architecture.puml` | Panels, widgets, transitions and the virtual-canvas scaler |
| `master_game_lifecycle.puml` | Kept from the original planning set; predates the current structure — treat as historical |

## Removed from this set

Three files were deleted because they described things that were planned but never built, and leaving
them here would misrepresent the project:

- `event_bus.puml` — there is no `EventBus` in the codebase, and no publish/subscribe mechanism.
- `object_pool.puml` — there is no object pool; projectiles and effects are allocated per spawn.
- `player_state_strategy.puml` — superseded by `player_state_machine.puml`, which reflects the
  behaviour classes that actually exist.

The older planning document `workflow/PLAN.md` also predates the current implementation — among other
differences it refers to SDL2, while the project uses raylib. It is retained as a record of the
original plan, not as a description of the code.

## Rendering these

Any of the following will render a `.puml` file:

- The **PlantUML** extension for VS Code (`Alt+D` to preview)
- The online renderer at <https://www.plantuml.com/plantuml>
- The command line, with Java and `plantuml.jar`:
  ```bash
  java -jar plantuml.jar workflow/blueprints/*.puml
  ```

The same diagrams appear as Mermaid inside `Group51/SuperMarioPlus_Design_Report.md`, which renders
directly on GitHub and in VS Code without any extension.
