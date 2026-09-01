# GROUP 51 — SUBMISSION INDEX

**Course:** CS202 – Programming Systems · **Class:** 25A01
**Project:** Super Mario Plus — 2D action-platformer (C++17 / raylib / LDtk)

| Student ID | Full name |
|---|---|
| 25125028 | Phạm Đức Minh |
| 25125007 | Lê Tiến Bình |

---

## What is in this folder

| # | Required by the brief | File | Contents |
|---|---|---|---|
| 1 | AI Usage Declaration | `AI_Usage_Declaration.md` | 20 documented conversations, each showing our first approach and its weakness, the exchange, and what we learned |
| 2 | The report, with class diagrams, applied design patterns and design reasoning | `SuperMarioPlus_Design_Report.md` | 13 sections · **49 diagrams** · the OOP rules, every class hierarchy, every applied pattern with its reasoning, all four game-loop phases, and the LDtk↔JSON↔C++ pipeline |
| 3 | List of features | `SuperMarioPlus_Feature_Catalog.md` | **99 features**, F001–F099, each explained with its mechanism and its source files |
| 4 | The source code | the repository root | `include/` · `src/` · `assets/` · `CMakeLists.txt` |
| 5 | Member contribution | `member_contribution.md` → paste into `member_contribution.xlsx` | 42 tasks with real commit dates |

> ### The feature count is 99
> Section 1.5 of the design report names four **architectural highlights**. Those are engineering
> themes, not the feature count. The full inventory of **99 features** is in
> `SuperMarioPlus_Feature_Catalog.md`, each with its own subsection explaining what it does, how it
> works, and which source files implement it.
>
> The catalogue also opens with a **required-topic map**, linking each topic named in the brief —
> PlayerInputsMovementCollision, EnemyBehavior, AI, PowerUpsItems, MultiplePlayers, map building,
> LevelCompletion, Sounds, OOD, DesignPatterns — to the features that realise it.

---

## Reading order we suggest

1. **Report §1–2** — what the game is, how it is played, and what it contains.
2. **Report §4** — the object-oriented rules and every class hierarchy, with diagrams.
3. **Report §5** — the design-pattern register, then each pattern with its reasoning, its
   consequences, and the four patterns we deliberately did *not* adopt.
4. **Report §6–8** — what every group of files is responsible for, the four game-loop phases traced
   end to end, and the key technical pipelines (LDtk↔JSON↔C++, collision, combat, save, editor).
5. **`SuperMarioPlus_Feature_Catalog.md`** — the 99 features in detail.
6. **`AI_Usage_Declaration.md`** — how the work was actually done.

---

## Supporting material in the repository

| Location | Contents |
|---|---|
| `workflow/blueprints/*.puml` | The same architecture as PlantUML source, verified against commit `b378acb` |
| `workflow/blueprints/README.md` | What each blueprint shows, and which older ones were removed and why |
| `structure.md` | Running notes on the project structure kept during development |
| `README.md` (repository root) | Build instructions |

---

## Building the project

No package manager is required — raylib is vendored as a prebuilt static library and nlohmann/json is
header-only, both under `third_party/`.

**Visual Studio (MSVC)**

```bash
cmake --preset windows-msvc
cmake --build build
.\build\Debug\SuperMarioPlus.exe
```

**MinGW-w64 (GCC)**

```bash
cmake --preset windows-mingw
cmake --build build
.\build\SuperMarioPlus.exe
```

If you switch between the two compilers, delete the `build` folder first.

---

## Producing the PDF versions

The brief asks for both Markdown and PDF. The Markdown files are final; any of these will produce the
PDFs:

**VS Code** — install the *Markdown PDF* extension, open the file, then
`Ctrl+Shift+P` → **Markdown PDF: Export (pdf)**. Mermaid diagrams render automatically.

**Pandoc** — if you have Pandoc and a LaTeX distribution:

```bash
pandoc SuperMarioPlus_Design_Report.md -o SuperMarioPlus_Design_Report.pdf --toc
```

Pandoc does not render Mermaid on its own. Either install the `mermaid-filter` package, or use the VS
Code route above, which needs no extra setup.

**Browser** — open the file on GitHub (Mermaid renders there natively) and print to PDF.

---

*Group 51 — Phạm Đức Minh (25125028) · Lê Tiến Bình (25125007) — September 2026*
