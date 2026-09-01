# AI USAGE DECLARATION

**Course:** CS202 – Programming Systems · **Class:** 25A01 · **Group:** 51
**Project:** Super Mario Plus — 2D action-platformer (C++17 / raylib / LDtk)
**Period covered:** 8 June 2026 → 1 September 2026

| Student ID | Full name | Commits |
|---|---|---|
| 25125007 | Lê Tiến Bình | 38 |
| 25125028 | Phạm Đức Minh | 30 |

---

## About this document

Below are twenty of the conversations we had with an AI coding assistant while building this project.
We used the assistant heavily and across almost every subsystem, so this is a selection rather than a
complete log.

Each entry is written to show the **thinking**, not just the answer. Every one records what our own
first approach was and **why it was not good enough**, because that is where the actual learning
happened — the assistant was most useful when it explained the weakness in an idea we already had,
and least useful when we asked it to write something we had not thought about ourselves.

Roughly a third of these entries are **bug investigations**: how the bug looked, what we wrongly
assumed at first, how we narrowed it down, what the real cause was.

Every piece of code discussed here was read, compiled, run and play-tested by us before it was
committed. Where the assistant gave us something wrong, that is recorded too — several entries end
with us rejecting the answer.

**How the work was divided.**

| Member | Area of ownership |
|---|---|
| **Lê Tiến Bình** | Level format and tile map, collision, camera framing, items, the whole user interface, the shop, the map editor, the save system, branch merges |
| **Phạm Đức Minh** | Player state machine, animation, skills and combos, combat resolution, projectiles, all enemies and bosses, enemy AI, cutscenes |

Every entry follows the same six-part shape:

> **Context** · **Our first approach and its weakness** · **Prompt** · **AI response** · **Plan → Adjust → Build → Evaluate** · **What we learned**

---

# Part 1 — Lê Tiến Bình

## B-01 · Deciding the shape of a frame

**Date:** 8–14 June 2026 · **Related commits:** `first commit`, `codebase & rules`, `update final codebase`

**Context.** We had an empty project and a raylib window. Before writing any gameplay we wanted to
decide how a frame would be organised, because we had both seen student projects where input reading,
physics and drawing are mixed inside one giant `update()` and become impossible to change.

**Our first approach and its weakness.** Our instinct was one method per object, `update(dt)`, doing
everything. We could already see the problem in a small prototype: an enemy read the keyboard in the
middle of its movement code, and when we wanted to pause the game we had to add an `if (paused)` in
about nine different places, and we still missed two.

**Prompt.**
> I am starting a 2D platformer in C++17 with raylib. I want to fix the structure of a frame before
> writing gameplay. My current design is a single `update(dt)` per object that reads input, decides
> what to do, moves things and draws. Pausing the game already needs guards in many places. What
> structure would avoid that, and what should the rule be for every object in the game, not just the
> screens?

**AI response.** It proposed splitting every per-frame object into four phases with strict rules about
what each may do: `HandleInput` reads devices and records intent only; `Process` makes decisions using
that intent but never touches devices; `Update(dt)` advances state and physics; `Render` only draws
and mutates nothing. It pointed out that pausing then becomes a single decision about which phases to
run, and that a fixed-timestep loop is only possible when exactly one phase advances state.

**Plan → Adjust → Build → Evaluate.** We adopted it but made it stricter than suggested: we applied it
to *entities* as well as screens, so an enemy has `decideAction()` in the Process phase and `update(dt)`
separately. We wrote the rule into the project's own documentation so we would both follow it. Building
a first walking character confirmed that pausing was one early return.

**What we learned.** The weakness in our original idea was not that it was untidy — it was that it made
one specific future change (pausing, and later the fixed timestep) structurally impossible without
touching everything. A constraint accepted early can be cheaper than the flexibility it removes. This
rule shaped every class we wrote afterwards.

---

## B-02 · Reading a professional level format

**Date:** 27–28 July 2026 · **Related commits:** `temp map`, `add physical logic for map01 and map02`

**Context.** We had been hand-writing levels as text files. That stopped scaling the moment we wanted
a level larger than one screen, so we moved to LDtk, a real level editor, and needed to parse its
project files.

**Our first approach and its weakness.** We began by reading the `.ldtk` file top to bottom in one
pass, building tiles as we encountered them. It failed on the first real level: tile layers reference
tilesets by a numeric identifier defined in a *different* section of the file, and depending on
ordering we were looking up tilesets that had not been read yet.

**Prompt.**
> I am parsing an LDtk project file with nlohmann/json. My single-pass parser breaks because tile
> layers reference tileset identifiers that are defined elsewhere in the file, so lookups fail
> depending on order. The game renders at 32-pixel tiles while LDtk authors at 16. Suggest a pass
> structure, and tell me which fields I must not assume are present.

**AI response.** It recommended a multi-pass structure: first collect the tileset definitions into a
map from identifier to texture, then the collision layer's integer grid, then the visual tile layers,
then the entity instances. It warned specifically that several fields are present but null rather than
absent — an entities layer has a tileset field set to null — and that reading such a field with a
convenience "value or default" call throws, because that call only substitutes for *missing* keys, not
for null ones.

**Plan → Adjust → Build → Evaluate.** We implemented the pass structure. The null warning was correct
and saved us: our first attempt did use the convenience call and would have thrown on every map. We
added explicit null checks. Our own bug remained afterwards — entities appeared at a quarter of their
correct position, because we had applied the 16-to-32 scale to tiles but forgotten to apply it to
entity positions.

**What we learned.** Two things. First, that "the key exists" and "the key has a value" are different
questions in JSON, and a library convenience function may only answer one of them. Second, that when a
format has cross-references, ordering is a design constraint on the parser, not a detail — the passes
exist because of the data's shape.

---

## B-03 · Terrain that is more than solid or empty

**Date:** 28–31 July 2026 · **Related commits:** `finished world03`, `update all map`

**Context.** With levels loading, we needed the world to actually behave: water to swim in, ladders to
climb, one-way platforms, lava, poison, pits that kill.

**Our first approach and its weakness.** We stored collision as a boolean grid — solid or not — and
planned to layer special behaviour on top with separate lists of rectangles for water, lava and so on.
By the third special surface we had five parallel containers to keep in step, and asking "what am I
standing on" meant querying all of them.

**Prompt.**
> My level collision is a boolean grid plus separate rectangle lists for water, lava and ladders, and
> it is becoming unmanageable. I need about a dozen distinct terrain behaviours. What representation
> should I use, and how should the entity react to the terrain without a long conditional in the
> physics code?

**AI response.** It recommended replacing the boolean with an enumeration of terrain types stored in
the same grid, so one query answers everything, and separating *detecting* the terrain from *reacting*
to it: the physics routine should run a fixed sequence and call named hooks — landed, hit a wall,
entered water, touched a hazard — that each entity overrides differently.

**Plan → Adjust → Build → Evaluate.** We adopted both parts and ended with thirteen terrain types in a
single grid. The hooks turned out to be more valuable than the grid change: the player's "entered
water" reaction is switching to a swimming behaviour, an enemy's "hit a wall" reaction is turning
around, and neither the physics code nor the two entities know about each other. Building world 3 with
water, ladders and one-way platforms exercised the whole thing.

**What we learned.** Our parallel containers were a symptom of thinking about *data* first. The real
question was "who decides what happens" — and once detection and reaction were separated, the data
representation became obvious. This is the Template Method structure described in §10 of our design
report; we arrived at it from a maintenance problem, not from a pattern catalogue.

---

## B-04 · Merging two branches that disagreed about the design

**Date:** 7–8 August 2026 · **Related commits:** `merge: binh01 into binh02 with OOP/DP refactor`, `The basic functions of the two branches have been merged`

**Context.** We had been working in parallel for weeks. One branch had the camera, tile map and the
new collision work; the other had skills, combat and projectiles. They had diverged in how a character
stores its physics and collision data, and merging was not a matter of resolving text conflicts.

**Our first approach and its weakness.** Our plan was to take one branch as the base and re-apply the
other's changes by hand. Trying it, we found we could not tell which version of the shared character
data structure was *correct* — both worked in their own branch because each branch's other code had
been adapted to it. Picking one meant silently breaking the other half.

**Prompt.**
> I have two branches to merge. Branch A owns the tile map, camera and a collision system using a
> bitmask, and added ladder and water timers to the character data. Branch B owns skills, combat and
> projectiles, and added a separate collision box for crouching to the same structure. I do not want
> to just pick a side — help me work out what the merged character data has to contain for both halves
> to remain correct, and list what else the merge forces me to change.

**AI response.** It worked through the two structures field by field and concluded that the additions
were independent rather than conflicting: the merged structure needs both the crouch box and the
collision mask and both timers, because they answer different questions. It then listed consequences
we had not thought about, including that two new movement behaviours would be needed for swimming and
climbing, and that a factory selecting characters with a chain of comparisons should become a lookup so
the merge did not multiply the branches.

**Plan → Adjust → Build → Evaluate.** We merged the structure as advised, added the swimming and
climbing behaviours, and converted the character factory to a lookup. We rejected one suggestion —
making a combat query method const by casting away constness internally — and instead changed the
method's signature, because the cast would have been undefined behaviour. Building and playing every
existing level end to end was the only way to be confident.

**What we learned.** The instinct to "pick the better branch" was wrong because it framed a merge as a
choice. Once we asked instead *what must be true for both halves to work*, most conflicts turned out
not to be conflicts. We also learned to be suspicious of an answer that makes a compiler error go away
without changing the meaning of the code — casting away constness compiled, and would have been a
latent bug.

---

## B-05 · An inventory with exactly one slot

**Date:** 18 August 2026 · **Related commits:** `add item's logic`

**Context.** The player needed to carry one item — a bomb, a flask or a power-up — and one key had to
do the right thing with whatever was held.

**Our first approach and its weakness.** We wrote it as a conditional on the item's name inside the
player: if it is a bomb, throw it; if it is poison, throw it differently; otherwise treat it as a
power-up. It worked for three items. It meant the player class had to know every item type that would
ever exist, and each new item was an edit to the player.

**Prompt.**
> The player carries one item and one key uses it. Right now `Player::useStoredItem` is a chain of
> string comparisons that decides whether to throw or consume. Every new item means editing the player
> class. How should I structure this so the player does not know the item types, and what should own
> the decision instead?

**AI response.** It suggested giving each *kind of use* its own small class behind a common interface
with a single `use(Player&)` method, and a factory that maps the held item's name to the right one. The
player then asks the factory for a behaviour and calls it, learning nothing about what happened.

**Plan → Adjust → Build → Evaluate.** We implemented three behaviours: consume a power-up, throw a
bomb, throw a flask. Adding the poison flask afterwards required no change to the player at all, which
was the test we cared about. We did hit one problem the assistant did not warn us about: an item whose
name has no matching behaviour leaves the slot occupied forever, so we made an unmatched item report
itself rather than fail silently.

**What we learned.** The chain of comparisons was not merely ugly; it put knowledge in the wrong place.
The question that unlocked it was "who *should* know this?" — and the answer was neither the player nor
the item, but a small object dedicated to the decision. This is the Strategy application described in
§12 of our report.

---

## B-06 · Two implementations of the same feature, in one merge

**Date:** 19 August 2026 · **Related commits:** `Merge minh1 into binh02 with conflict resolution`, `Fix merge compilation errors`

**Context.** A second merge, and this time both branches had implemented overlapping combat behaviour
in different ways.

**Our first approach and its weakness.** We tried to keep both, guarded by flags, on the theory that
this was the safe option. The result compiled and was much worse than either: two systems partially
active, each half-correct, and behaviour that depended on evaluation order.

**Prompt.**
> Both branches implement how an attack applies damage, differently. I tried keeping both behind
> flags and the result is worse than either. Do not just tell me to pick one — compare them against
> how the rest of the code uses them and tell me which is actually consistent with the rest of the
> system, and what has to be deleted.

**AI response.** It traced how each version was used elsewhere and found that one of them assumed a
hitbox stayed valid across a whole animation while the surrounding code recreated it per frame. That
version could not be correct in this codebase regardless of its merits in isolation. It recommended
taking the other, and named the specific variables that became dead once the first was removed.

**Plan → Adjust → Build → Evaluate.** We removed the losing version entirely, deleted the dead
variables, and fixed the compilation errors the removal exposed. Playing through the existing levels
confirmed the combat behaved as it had on the branch we kept.

**What we learned.** Keeping both implementations felt cautious and was actually the riskiest choice —
it produced a system neither of us understood. The useful question was not "which is better" but
"which is consistent with the assumptions the rest of the code already makes". A design is only
correct relative to its surroundings.

---

## B-07 · Menus, settings, and controls the player can change

**Date:** 29–30 August 2026 · **Related commits:** `add menu UI, settings & map building`, `2 map selection UI & characters selection UI`, `debug for player selection UI`

**Context.** The game needed a real front end: a main menu, world selection, character selection, and a
settings screen where both players could rebind all eleven of their controls.

**Our first approach and its weakness.** Our input code read specific keys directly at the point of
use. Rebinding would have meant threading a configuration lookup through dozens of call sites, and a
second player would have meant duplicating all of them.

**Prompt.**
> I need eleven rebindable actions per player, for two players sharing one keyboard, with the bindings
> saved between sessions. Right now the code reads specific keys where the action happens. What
> structure lets a key be reassigned at runtime without editing the player class, keeping in mind that
> some actions fire once on press, some repeat while held, and one action takes a parameter — "use
> skill" needs to know *which* skill?

**AI response.** It proposed turning each action into an object with an `Execute(Player&)` method, held
in a map from key code to a list of action objects together with the trigger type. It specifically
noted that the parameterised action is why function pointers are not enough — an object can carry the
skill name as a member.

**Plan → Adjust → Build → Evaluate.** We built it, and two players became two input handlers
constructed from two sets of key codes. The settings screen writes a key code and nothing else in the
game learns about it. We then spent a debugging session on the character selection screen, where
confirming a hero occasionally selected the wrong one — the cause was our own: the click was being
consumed in the same frame that the screen opened.

**What we learned.** The parameterised action was the detail that decided the design. We had been
thinking of "input handling" as a lookup problem, and it is really a *representation* problem: an
action is a thing with data, not just a function to call. That reframing is what made rebinding
trivial afterwards.

---

## B-08 · A level editor inside the game

**Date:** 29 August 2026 · **Related commits:** `add menu UI, settings & map building`

**Context.** We wanted players to be able to build their own levels, using the same blocks and enemies
as the campaign, and play them immediately.

**Our first approach and its weakness.** Our first editor stored the map as a dense two-dimensional
array of block names and had each editing action record how to reverse itself, so that undo could
replay the inverse. Painting undid correctly. Resizing did not: shrinking the map drops a row of tiles
*and* shifts every entity, and our inverse for that operation was wrong in a way that only showed up
two or three undos later, by which point the map was quietly corrupted.

**Prompt.**
> My map editor has undo implemented as a per-operation inverse. Painting works, but resizing the map
> is a compound operation — it changes the dimensions, re-indexes every stored tile and moves every
> entity — and my inverse for it is wrong in a way that corrupts the map several undos later. Should I
> fix the inverse or change the approach? The map data is a grid of block names.

**AI response.** It recommended abandoning inverses in favour of snapshotting the whole map before
each change, and pointed out that this is only affordable if the map is stored *sparsely* — only the
cells that are not empty — rather than as a dense grid. It noted the consequence we would have to
handle: if the cell index is derived from the map width, every resize must re-index every stored cell.

**Plan → Adjust → Build → Evaluate.** We changed the storage to sparse and replaced inverses with
snapshots. Undo became correct by construction, and resizing became undoable without any extra work.
One refinement we added afterwards: a snapshot is only kept if the operation actually changed
something, because otherwise clicking on a cell that already held the selected block filled the history
with nothing and made undo appear broken.

**What we learned.** We had been optimising memory without measuring, and paying for it in correctness.
The sparse representation made the "expensive" option cheap, which changed which design was even
available. When an approach keeps producing subtle bugs, the productive question is whether a
different approach makes the bug *impossible* rather than whether this one can be fixed again.

---

## B-09 · A shop that pauses the world

**Date:** 31 August 2026 · **Related commits:** `finished build shop UI & trace system`

**Context.** We added a shop the player walks into, which opens a purchase screen over the level.

**Our first approach and its weakness.** We drew the shop over the level and left the level running
underneath. Enemies kept moving and could kill the player while they were browsing, and the player
could still be hit by a projectile fired before the shop opened.

**Prompt.**
> I have a shop panel drawn over a running level. The world keeps simulating underneath, so the player
> can be killed while shopping. Given my four-phase frame structure, where exactly should I stop the
> world so the shop still animates and responds to the mouse, but nothing in the level moves?

**AI response.** It pointed out that our own phase separation already answered this: return early from
the level's `Update` phase while the shop is open, since that is the only phase that advances state,
and let `HandleInput` and `Render` continue to run. It warned to place the early return *after* any
timers the interface itself depends on.

**Plan → Adjust → Build → Evaluate.** One early return in the right place froze the world completely
while the shop stayed fully interactive. The warning about timer placement was relevant — our first
attempt put the return too early and the shop's own animation froze along with the world.

**What we learned.** The answer came from a rule we had set ourselves in June and had stopped thinking
about. A structural constraint you adopt early keeps paying out later, but only if you remember to ask
it. We had been about to add a "paused" flag threaded through the level, which the phase rule made
unnecessary.

---

# Part 2 — Phạm Đức Minh

## M-01 · Ten behaviours without nested conditionals

**Date:** 12–14 July 2026 · **Related commits:** `create state flow`, `Finish state flow`, `player state build`

**Context.** The player needed to be idle, running, jumping, falling, crouching, hurt, dying and using
a skill — each interpreting input differently and each with work to do on entry and exit.

**Our first approach and its weakness.** We wrote it as flags: `isJumping`, `isAttacking`, `isHurt`,
and conditionals over them in `update`. Two problems appeared quickly. Invalid combinations were
representable — the code could believe the player was crouching and swimming at once — and the entry
work for each behaviour was duplicated at every place that could cause it.

**Prompt.**
> My player uses boolean flags for jumping, attacking, hurt and so on, and the update method is now a
> pyramid of conditionals. Two flags can be true at once when they should not be. I need about ten
> behaviours, each interpreting the same input differently, and some of them must not be interruptible
> — an attack has to finish. What structure should I use?

**AI response.** It proposed making each behaviour a class with entry, exit and update methods plus one
handler per input action, and giving the character a pointer to the current behaviour. It added an
interruption guard so a behaviour can refuse to be exited, which is what the attack needed.

**Plan → Adjust → Build → Evaluate.** We built ten behaviour classes. The invalid-combination problem
disappeared entirely: with one current behaviour, "crouching and swimming" is not expressible. We made
one change to the proposal — behaviour objects are stored *by value* inside the player rather than
allocated on each transition, because the player changes behaviour many times per second.

**What we learned.** The flags were not just untidy; they allowed states that do not exist in the
game. Choosing a representation in which invalid states cannot be written down is stronger than
checking for them. The interruption guard was the piece we would not have thought of, and it is what
made attack animations feel solid rather than cancellable.

---

## M-02 · Animation that survives inconsistent artwork

**Date:** 20 July 2026 · **Related commits:** `add Animation`, `add attack animation`

**Context.** Six characters, seventeen animations each, drawn as horizontal sprite strips of differing
frame sizes.

**Our first approach and its weakness.** We assumed every frame in every strip was the same size and
computed the source rectangle by multiplying the frame index by a constant. It broke as soon as we
added a second character whose artwork used a different frame size, and worse, it broke *silently* —
the animation played, showing slices of adjacent frames.

**Prompt.**
> My animation code assumes a constant frame width and computes the source rectangle by index. With
> multiple characters whose sprite strips have different frame sizes, it silently shows slices of the
> wrong frames. I do not want to re-export the art. How should the animation get its frame size, and
> how do several characters share this code?

**AI response.** It recommended that frame dimensions be part of the animation's own data rather than a
global constant, derived from the strip's dimensions and the declared frame count, and that the frame
count come from the configuration file alongside the texture name and the timing.

**Plan → Adjust → Build → Evaluate.** We moved frame count and timing into the character configuration.
Adding Luffy two days later was a configuration edit. Later, Sasuke needed twenty-one animations rather
than seventeen and that required no code change either, which is the point at which we knew the design
was right.

**What we learned.** A constant that "is always true right now" is a bug scheduled for later. The real
lesson was about failure mode: this bug produced plausible-looking wrong output rather than a crash,
which took far longer to notice. We started preferring designs where wrong data fails loudly.

---

## M-03 · A combo chain that stays data

**Date:** 1 August 2026 · **Related commits:** `finish basic skill`

**Context.** Repeated attack presses should chain four different attacks, each with its own animation,
damage and timing, and the chain should reset if the player waits too long.

**Our first approach and its weakness.** We tracked a combo counter in the player and used a conditional
to pick the attack for the current count. The player class then had to know how many attacks existed
and in what order, and the timings lived in a different place from the attacks themselves.

**Prompt.**
> I have a four-hit melee combo implemented as a counter in the player plus a conditional selecting
> which attack to play. The player now knows the whole chain, and the timing values are separated from
> the attacks. How do I make the chain data rather than control flow, given each attack has its own
> animation, damage, and window during which it can hit?

**AI response.** It suggested that each attack declare which attack may follow it, so the chain is a
property of the attacks rather than of the player, and that each attack carry its own timing — how long
it lasts, and between which moments its damage is active.

**Plan → Adjust → Build → Evaluate.** We gave each attack a "next in chain" name and its own timings.
The player's role shrank to "ask the current attack what may follow". Changing the combo order became
a change to two attack classes and nothing else. We loaded the timing numbers from configuration so
they could be tuned without rebuilding.

**What we learned.** The combo counter was the player storing knowledge that belonged to the attacks.
Once each attack knew its own successor, the chain became extensible — adding a fifth attack does not
touch the player. This is the same lesson as B-05 from a different direction: ask what *should* know a
fact, not what *can*.

---

## M-04 · Projectiles that do not shoot their owner

**Date:** 6 August 2026 · **Related commits:** `add fireball`

**Context.** Characters and some enemies fire projectiles that travel, collide, damage and expire.

**Our first approach and its weakness.** A projectile was spawned directly into the level's entity list
from inside the attack. Two problems. Adding to the list while it was being iterated invalidated the
iteration and crashed intermittently — which we initially misdiagnosed as a physics bug because the
crash location varied. And a projectile spawned at the character's own position immediately collided
with the character that fired it.

**Prompt.**
> Spawning a projectile from inside an attack crashes intermittently, in different places each time,
> and when it does not crash the projectile damages the character that fired it. The projectile is
> added directly to the level's entity vector from the player's update. What is actually wrong here?

**AI response.** It identified both. The intermittent crash is iterator invalidation — appending to a
vector while iterating it may reallocate, leaving the loop's iterator dangling — and the varying crash
location is the signature of that, not a physics problem. It recommended that the entity describe what
it wants to spawn and put that description in a queue the level drains at a safe point, and that the
description carry a reference to the spawner so collision can skip it.

**Plan → Adjust → Build → Evaluate.** We built the spawn queue and added the spawner reference. The
crash disappeared. The design had an unexpected benefit: entities no longer needed to know about the
level at all, which removed a dependency we had been uncomfortable with.

**What we learned.** The most valuable part was the diagnosis, not the fix. We had been looking at
physics for two days because that is where the crash appeared. Learning that *"crashes in a different
place each run"* is a fingerprint of memory being invalidated rather than of the code at the crash site
changed how we debug. This became the message queue described in §15 of our report.

---

## M-05 · Making hits land when the animation says they do

**Date:** 18 August 2026 · **Related commits:** `debug skill`, `animation for health/attack`, `fix combatsystem`

**Context.** Attacks were connecting at the wrong time — damage was applied the instant an attack
started, before the character had visibly swung.

**Our first approach and its weakness.** The attack created its damage area when it began and destroyed
it when it ended. That made the whole animation dangerous, including the wind-up and the recovery, so
an enemy standing next to the player took damage from an attack that had not visually happened yet.

**Prompt.**
> My attacks apply damage for the entire animation, so a hit lands during the wind-up before the swing
> is visible. I want damage to be active only during specific frames of each attack. Where should that
> window live, and how should the combat system find which hitboxes are active this frame?

**AI response.** It recommended that each attack declare a start and end moment for its damage window
as part of its own data, that the entity report whether it currently has an active damage area, and
that the combat system collect all active areas each frame and resolve them centrally rather than each
attack resolving its own.

**Plan → Adjust → Build → Evaluate.** We added the window to each attack's data and centralised
resolution. Hits started landing when they looked like they should. Centralising also gave us somewhere
to put the rule that allies cannot damage each other, which had previously been duplicated.

**What we learned.** We had been treating an attack as a *duration* when it is really a *sequence* with
distinct phases — anticipation, active, recovery. Naming those three phases changed how every
subsequent move was designed. Centralising resolution was the second lesson: when the same rule is
being written in several places, that rule wants to live somewhere else.

---

## M-06 · Enemies that behave like they can see

**Date:** 22–27 August 2026 · **Related commits:** `enemy plan`, `mobs add`, `mob fixed`, `fixed mob`

**Context.** Nine enemy types needed to patrol, notice the player, chase, attack, react to damage and
die — and they needed to differ from each other.

**Our first approach and its weakness.** We wrote the behaviour once with per-enemy constants. It was
mostly fine until we tried to make a flying enemy and a stationary tree behave differently: both needed
to *not* do things the shared code always did, so the shared method filled with exceptions.

**Prompt.**
> I have nine enemy types sharing one AI update with per-type constants, and the exceptions for a
> flying enemy and a stationary one are taking over the method. I already use per-object behaviour
> classes for the player. Should enemies use the same approach, and if so what is different about
> them? Also, enemies keep walking into water and off ledges.

**AI response.** It recommended the same behaviour-object structure as the player but stored
differently — allocated per transition rather than kept resident — because there are many enemies and
each uses only a few of its behaviours at a time. For the walking-into-hazards problem it suggested
that the physics already knows when an entity is about to leave solid ground or enter liquid, so the
fix is a hook the cautious enemies override rather than a check inside the AI.

**Plan → Adjust → Build → Evaluate.** We built seven behaviours for enemies and marked which enemy
types are cautious about ledges and liquid. The behaviour-storage difference mattered: with forty
enemies in a room, keeping every behaviour resident for each would have been wasteful.

**What we learned.** The same pattern can be *implemented* two ways in one project for good reasons,
and being able to explain why is more valuable than being consistent for its own sake. The hazard fix
also reinforced M-03's lesson: the ledge check did not belong in the AI, because the physics already
had the information.

---

## M-07 · Taking the camera away and giving it back

**Date:** 27 August 2026 · **Related commits:** `add camera pan and cutscene`, `fix camera`

**Context.** Story moments needed the camera to leave the player, move to something important, hold
while dialogue played, and return.

**Our first approach and its weakness.** We added flags to the camera: whether it was panning, where to,
how long it had been going, whether it should return afterwards. The update method became a thicket,
and a second kind of shot — zooming — doubled it. Worse, if a cutscene was interrupted the flags were
left set and the camera stayed stuck away from the player.

**Prompt.**
> My camera has flags for panning and zooming and the update method has become unmanageable, and an
> interrupted cutscene leaves the camera stuck. Shots need to compose: pan, then zoom, then return to
> following. What structure handles a sequence of camera behaviours, and how should "following" fit
> into it?

**AI response.** It recommended making each camera behaviour an object that moves the camera and
reports when it is finished, with the camera holding the current one and a queue of pending ones.
Crucially it suggested that *following* be one of those behaviours rather than a special default, so
returning to normal is just another entry in the queue.

**Plan → Adjust → Build → Evaluate.** We built three behaviours — follow, pan, zoom — and a queue. A
cutscene now composes shots by pushing behaviours. The stuck-camera bug disappeared because
interruption is clearing the queue rather than unsetting a set of flags that must all be remembered.

**What we learned.** Treating the normal case as "just another case" removed the special-casing that
was causing the bug. We had been thinking of following as the camera's *nature* and panning as a
temporary deviation; making them peers was what simplified it.

---

## M-08 · A boss whose hits missed, and the intelligence to jump

**Date:** 29–31 August 2026 · **Related commits:** `add boss`, `fix boss hitbox`, `add Jump AI`, `add dash mul`, `full boss & mob`, `fix AI boss`

**Context.** Six bosses with a scripted entrance, seven moves each, and enough intelligence to be worth
fighting.

**Our first approach and its weakness.** Bosses reused the enemy damage area sizing, which was
proportional to the enemy's sprite. Boss sprites are much larger, and some boss animations are drawn on
larger canvases than others, so a boss's damage area was sometimes far larger than the boss appeared
and sometimes offset from it. Players were being hit by attacks that visually missed.

Separately, bosses could not follow a player onto a platform, so a fight could be trivialised by
standing on a ledge.

**Prompt.**
> My bosses' attack areas do not match their sprites — some animations are drawn on larger canvases
> than others, so the damage area is bigger than the visible boss and offset from it. I do not want to
> re-export the art. Second problem: bosses cannot reach a player standing on a platform, so the fight
> can be trivialised. How should the attack area be defined, and what should trigger a jump?

**AI response.** For the first, it recommended defining the damage area explicitly per attack in the
configuration rather than deriving it from sprite size, so the artwork's canvas becomes irrelevant. For
the second, it suggested that the decision to jump come from comparing the target's height against the
boss's, gated by a cooldown so the boss does not jump continuously.

**Plan → Adjust → Build → Evaluate.** We moved attack areas into per-attack configuration and added the
height-based jump decision with a cooldown. The first change fixed the mismatch completely and let us
tune each attack's reach independently. The jump decision needed adjusting after playtesting — our first
threshold made bosses jump almost constantly, so we raised it and lengthened the cooldown.

**What we learned.** Deriving one thing from another because they happened to correlate — damage area
from sprite size — coupled our combat design to our art pipeline. Once the two were separated, both
became easier to change. The AI tuning taught us something different: a behaviour that is *correct* can
still be *bad*, and only playing it tells you which.

---

# Part 3 — Final phase, worked on together

## J-01 · Saving a world, not just a player

**Date:** 1 September 2026 · **Related commit:** `Fix potential project bugs, upgrade the character selection screen UI, implement game backup, and handle the ending state`

**Context.** Checkpoints needed to write real save files the player could return to.

**Our first approach and its weakness.** Our first save recorded the player: health, position, coins.
Reloading restored the player into a world that had been fully reset — every enemy the player had
already defeated was alive again, every chest they had opened was closed. The save was correct and
useless.

**Prompt.**
> My save file records the player's stats and position, but reloading resurrects every enemy the player
> already defeated and closes every chest they opened. How do I record the state of the *world*, given
> enemies and items are created fresh from the level file each time it loads? Also, our save writes
> directly over the previous file, and I am worried about a crash mid-write.

**AI response.** It observed that the level format already assigns every placed object a unique
identifier that does not change between sessions, so the save can record a set of defeated identifiers
and a map of changed item states, and level construction can consult both — skipping creation of
anything in the defeated set. For the write concern it recommended writing to a temporary file and
renaming it over the target, since a rename is atomic and a crash therefore leaves the previous save
intact.

**Plan → Adjust → Build → Evaluate.** We implemented both. We went further than suggested on structure,
separating the save into four layers — the data itself, a facade, a storage layer and a format layer —
so the format could be replaced. That let us exercise the save logic without touching a real filesystem
layout. We also added a small metadata header so the load screen could list ten saves without parsing
ten complete files.

**What we learned.** Our first save answered "where was the player" when the real question was "what had
already happened". The identifier insight was the key: we already had stable identity for every object
and had not realised it was the thing that made world state saveable. The atomic-rename technique was
new to us and is now something we would apply to any file a program cannot afford to corrupt.

---

## J-02 · An animated background, and a bug we caused ourselves

**Date:** 1 September 2026 · **Related commit:** `Fix potential project bugs, upgrade the character selection screen UI, implement game backup, and handle the ending state`

**Context.** We wanted an animated background on the character selection screen and an animated gate at
the end of each level, both authored as GIFs.

**Our first approach and its weakness.** We loaded the GIF the same way as any other image. It rendered,
but it never moved — a normal image load takes only the first frame of an animated GIF, which is not an
error and produces no warning.

**Prompt.**
> Loading a GIF the same way as a PNG shows only the first frame and never animates, with no error. How
> do I play an animated GIF, and how should I wrap it so the same code works for a full-screen
> background and for an object inside the level?

**AI response.** It explained that animated GIFs need a different loading call that returns all frames
in one buffer, and that advancing the animation means uploading the correct slice of that buffer to the
texture. It recommended wrapping this in a small class owning both the frame data and the texture, and
warned that the frame data has to stay in memory for the animation's whole life.

**Plan → Adjust → Build → Evaluate.** We built the wrapper and used it in both places. One of our GIFs
then crashed the program during loading — a defect in the image decoder for that particular file, not
in our code. We re-encoded the file, which fixed the crash, and in doing so **we broke it ourselves**:
our re-encoding dropped the transparency, so the gate rendered on an opaque black rectangle. The second
attempt preserved transparency by reserving a palette entry for it, and we verified the result by
comparing the re-encoded frames against the originals pixel by pixel before accepting it.

**What we learned.** Two lessons, and the second is the one we will remember. First, that a library
function silently doing *less* than you assumed is a whole category of bug — no error, plausible
output, wrong behaviour. Second, that our fix introduced a worse bug than the one we were fixing, and
we only caught it because we looked at the result. Verifying the output of a transformation is not
optional, especially when the transformation is something you wrote in a hurry to unblock yourself.

---

## J-03 · An interface that only looked right on our machine

**Date:** 1 September 2026 · **Related commit:** `Fix potential project bugs, upgrade the character selection screen UI, implement game backup, and handle the ending state`

**Context.** Our menus were laid out in absolute pixel positions, tuned by eye until they looked
correct.

**Our first approach and its weakness.** They looked correct on the machine we tuned them on and
nowhere else. On a smaller display the fixed-width panels consumed the entire width and the content
area between them collapsed; on a larger one everything clustered in a corner. Our first fix — scaling
by width and height independently — kept everything on screen but stretched circular buttons into
ovals.

**Prompt.**
> My interface is laid out in absolute pixels and only looks right at the resolution I tuned it at.
> Scaling the axes independently stops things overflowing but distorts the artwork. I want the layout
> to keep its proportions on any display without stretching anything. What is the standard approach,
> and what breaks if I get it wrong?

**AI response.** It described the virtual-canvas technique: lay everything out against one fixed
reference resolution, then scale that canvas by a single factor — the smaller of the two axis ratios,
so nothing distorts — and centre it, letting the leftover space become margins. It warned that
converting a *length* and converting a *position* are different operations, and that mixing them up
places elements exactly one margin-width out.

**Plan → Adjust → Build → Evaluate.** We implemented it. Our first attempt was still wrong on our own
machine, in a way we did not expect: we had chosen the reference resolution from the window size our
code requested at startup, but the game maximises immediately, and the display was running at 200%
scaling — so the window the game actually received was 2560×1459 physical pixels while the operating
system reported 1280×800. We measured the real value at runtime, set the reference accordingly, and
confirmed the scale factor came out at exactly 1.0 on our machine, meaning the interface was unchanged
where it had been tuned and proportional everywhere else.

**What we learned.** The technique was straightforward; the trap was believing a number we had never
verified. We had assumed the resolution we asked for was the resolution we got, and two different
layers — window maximisation and display scaling — had each changed it. Measuring at runtime instead of
assuming is now our default, and the length-versus-position distinction is a mistake we made once and
will not repeat.

---

# How we used the assistant

| Category | Where |
|---|---|
| **Architecture and design advice** | Frame structure (B-01), terrain representation (B-03), input as objects (B-07), player behaviours (M-01), combo chaining (M-03), camera sequencing (M-07), save layering (J-01) |
| **Diagnosing a bug we could not explain** | Intermittent projectile crash (M-04), attacks landing during wind-up (M-05), silent animation slicing (M-02), stuck camera (M-07), boss hitbox mismatch (M-08), GIF not animating (J-02), interface wrong on every other machine (J-03) |
| **Reviewing our approach and telling us it was wrong** | Undo by inverse (B-08), keeping both merge implementations (B-06), item-type conditionals (B-05), boolean state flags (M-01) |
| **Merge and integration analysis** | Character data merge (B-04), conflicting combat implementations (B-06) |
| **Not used for** | Level design and map authoring, art and audio selection, gameplay balance numbers, and the writing of our report documents |

## What we did not accept

- **Casting away constness** to silence a compiler error during the first merge (B-04). It compiled and
  would have been undefined behaviour. We changed the method signature instead.
- **Keeping both implementations behind flags** during the second merge (B-06). We tried it, produced a
  system neither of us understood, and deleted one entirely.
- **Deriving attack areas from sprite dimensions** (M-08), which is what we had already been doing and
  what caused the bug.
- **Our own first re-encoding of an image asset** (J-02), which fixed a crash and destroyed
  transparency. We rejected our own fix after checking the output.

## Skills we gained

- Recognising that a crash whose location varies between runs points at invalidated memory, not at the
  code where it crashes.
- Preferring representations in which invalid states cannot be expressed over code that checks for them.
- Asking "what *should* know this fact" rather than "what *can* reach this fact" when deciding where
  logic belongs.
- Treating a library function that silently does less than expected as a distinct category of bug worth
  looking for.
- Writing files through a temporary and a rename when corruption is unacceptable.
- Verifying the output of any transformation we write, rather than trusting that it did what we meant.
- Measuring the environment at runtime instead of assuming the values we asked for are the values we got.

---

*Group 51 — Phạm Đức Minh (25125028) · Lê Tiến Bình (25125007)*
*CS202 – Programming Systems · Class 25A01 · 1 September 2026*
