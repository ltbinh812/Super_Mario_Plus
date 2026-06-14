# Changelog - 12/06/2026

B+ío c+ío tiß¦+n tr+¼nh cß¦¡p nhß¦¡t kiß¦+n tr+¦c v+á t+¡nh n-âng cho dß+¦ +ín **Super Mario Plus**.

## 1. Cß¦¡p Nhß¦¡t Vß+ü Hß+ç Thß+æng Nh+ón Vß¦¡t & Kß+¦ N-âng (OOP)
- +üp dß+Ñng **Factory Pattern** th+¦ng qua `CharacterFactory` -æß+â quß¦ún l++ viß+çc sinh ra c+íc nh+ón vß¦¡t. -É+ú thiß¦+t kß¦+ th+ánh c+¦ng +¡t nhß¦Ñt 5 nh+ón vß¦¡t (Mario, Luigi, Peach, Toad, Wario), mß+ùi nh+ón vß¦¡t -æß+üu kß¦+ thß+½a tß+½ lß+¢p `Player`.
- +üp dß+Ñng **Strategy Pattern** cho hß+ç thß+æng Kß+¦ n-âng th+¦ng qua giao diß+çn `AbilityStrategy`. C+íc kß+¦ n-âng hiß+çn tß¦íi gß+ôm c+¦:
  - `DashAbility`: L¦¦ß+¢t nhanh vß+ü ph+¡a tr¦¦ß+¢c. -É+ú -æ¦¦ß+úc t-âng lß+¦c l¦¦ß+¢t (tß+½ 800 l+¬n 1200) -æß+â bay xa h¦ín. C+¦ thß+â kß¦+t hß+úp sß+¡ dß+Ñng ngay cß¦ú khi -æang chß¦íy hoß¦+c l¦¦ß+¢t tr+¬n kh+¦ng trung.
  - `HighJumpAbility`: Nhß¦úy si+¬u cao. Chß+ë -æ¦¦ß+úc ph+¬p k+¡ch hoß¦ít khi nh+ón vß¦¡t -æang chß¦ím -æß¦Ñt (-æß+â tr+ính spam nhß¦úy li+¬n tß+Ñc tr+¬n kh+¦ng).
  - `FireballAbility`, `GroundPoundAbility`, `FloatAbility` -æ+ú -æ¦¦ß+úc thiß¦+t kß¦+ sß¦¦n cß¦Ñu tr+¦c.
- Cß¦úi tiß¦+n th+¦ng sß+æ vß¦¡t l++ ri+¬ng biß+çt cho tß+½ng nh+ón vß¦¡t th+¦ng qua biß¦+n `gravity_` (v+¡ dß+Ñ: Peach trß+ìng lß+¦c cß+¦c thß¦Ñp n+¬n nhß¦úy rß¦Ñt cao v+á l¦í lß+¡ng, Wario trß+ìng lß+¦c nß¦+ng n+¬n r¦íi nhanh).

## 2. Cß¦¡p Nhß¦¡t Vß+ü Giao Diß+çn & -Éiß+üu Khiß+ân (UI/Input)
- Hß+ù trß+ú **2 ng¦¦ß+¥i ch¦íi (Co-op)** c+¦ng l+¦c tr+¬n b+án ph+¡m bß¦¦ng cß¦Ñu tr+¦c `InputConfig`. P1 d+¦ng cß+Ñm `W-A-S-D` (Kß+¦ n-âng: `J, K`), P2 d+¦ng cß+Ñm `Up-Left-Down-Right` (Kß+¦ n-âng: `Numpad 1, Numpad 2`).
- Ph+¡m `W/Up` -æß+â nhß¦úy b+¼nh th¦¦ß+¥ng (-æ+ú -æ¦¦ß+úc tinh chß+ënh logic lß+¦c nhß¦úy -æß+â c+¦ thß+â -æiß+üu khiß+ân -æß+Ö cao dß+¦a tr+¬n thß+¥i gian giß+» ph+¡m).
- Ph+¡m `S/Down` -æß+â c+¦i ng¦¦ß+¥i (giß¦úm hitbox) v+á tr¦¦ß+út chß¦¡m lß¦íi.
- +üp dß+Ñng **Strategy/Overlay Pattern** cho UI:
  - Ph+¡m `U`: Bß¦¡t giao diß+çn Overlay -æß+â -æß+òi nh+ón vß¦¡t cho Player 1 hoß¦+c Player 2 ngay trong trß¦¡n -æß¦Ñu (bß¦¦ng c+íc ph+¡m A/D v+á Left/Right).
  - Ph+¡m `Esc`: Bß¦¡t m+án h+¼nh Settings Overlay (vß+¢i c+íc n+¦t Save, Load, Menu).

## 3. Sß+¡a Lß+ùi Cß+æt L+¦i (Bug Fixes)
- Xß+¡ l++ dß+¬t -æiß+âm lß+ùi nhß¦Ñp nh+íy (flickering) khi nh+ón vß¦¡t chß¦ím -æß¦Ñt. Trß+ìng lß+¦c giß+¥ -æ+óy lu+¦n k+¬o nhß¦¦ nh+ón vß¦¡t xuß+æng ngay cß¦ú khi -æ+ú -æß+¬ng tr+¬n mß¦+t -æß¦Ñt -æß+â hß+ç thß+æng nhß¦¡n diß+çn va chß¦ím (Collision) li+¬n tß+Ñc trß¦ú vß+ü `onGround_ = true`. -Éiß+üu n+áy gi+¦p c+íc lß+çnh Nhß¦úy v+á C+¦i -ân 100%.
- T+íi cß¦Ñu tr+¦c lß¦íi code di chuyß+ân ngang (`HandleHorizontalMovement`) -æß+â ng¦¦ß+¥i ch¦íi c+¦ thß+â sß+¡ dß+Ñng Kß+¦ n-âng (Dash) tr¦ín tru ngay cß¦ú khi -æang giß+» n+¦t chß¦íy. Lß+¦c l¦¦ß+¢t kh+¦ng c+¦n bß+ï g+¦ +¬p giß+¢i hß¦ín vß¦¡n tß+æc ngay lß¦¡p tß+¬c nß+»a m+á sß¦+ tr+¦i dß¦ºn dß¦ºn (decelerate).

## 4. Refactoring Codebase
To+án bß+Ö source code -æ+ú -æ¦¦ß+úc dß+ìn dß¦¦p v+á chia nhß+Å v+áo 6 folder domain-driven (-æ+ú t+¡ch hß+úp tß+¦ -æß+Öng bß¦¦ng `CMakeLists.txt`):
- `core/`: C+íc lß+¢p cß+æt l+¦i (`GameManager`, `CameraManager`, `SuperMarioPlus`).
- `states/`: Quß¦ún l++ logic m+án ch¦íi, menu.
- `characters/`: Lß+¢p `Player` v+á c+íc nh+ón vß¦¡t.
- `abilities/`: Chiß¦+n l¦¦ß+úc kß+¦ n-âng.
- `ui/`: C+íc Overlay v+á HUD.
- `environment/`: M+óy, Nß+ün, Cß+Öt, Vß¦¡t cß¦ún...



## Ch¦¦a update xong
- Ch¦¦a c+¦ asset c+íc nh+ón vß¦¡tm k-¬ n-âng nh¦¦ fireball,...
- Ch¦¦a xß+¡ l+¡ va chß¦ím
