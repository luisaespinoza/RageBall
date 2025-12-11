# Dodgeball Game README

## **Controls**
- **W A S D** – Movement (Forward / Left / Backward / Right)
- **Left Mouse** – Launch Dodgeball

---

## **Contributions**

### **Luis –**
#### Core Systems & Architecture
- **Arena System**  
  - `_arenaRoom.h` → Room configuration, transforms, world/local conversions, midline logic, clamping, obstacle composition.

- **Spatial Navigation Framework**  
  - `_spatialNav.h` → Universal world↔local conversions, yaw mapping, movement clamping, muzzle placement helpers.

- **Geometry / Common Structs**  
  - `_geometryUtils.h` → Core `Pose`, `Obstacle` definitions shared across levels.

#### Characters & Combat
- **Base Character System**  
  - `_character.h` → Shared physics, health logic, projectile functions, muzzle computation, ball update.

- **Enemy AI System**  
  - `_enemy.h` → State machine (Patrol / Chase / Windup / Throw / Cooldown / Stunned), sight/FOV checks, chase logic, projectile spawning, arena clamping.

- **Projectile Trajectories**  
  - `_trajectories.h` → Straight, parabolic, lob, and bounce trajectory functions.

- **Player System Enhancements**  
  - `_player.h` → Movement, OBJ/MD2 hybrid rendering, animation blending, collision radius scaling, ball throw mechanics, ball update.

#### Level Logic & Progression
- **Level01 Gameplay Loop**  
  - `_level01.h` → Arena logic, enemy spawning, survival timer, hazards, input/camera routing, throw mechanics, timers, collision flow, multi-enemy behavior.

- **Hallway System (Level Structure)**  
  - `_hallway.h` → Modular hallway segments, texturing, chaining, yaw alignment, obstacle logic, world↔local movement clamping.

#### Game Flow / Scene Infrastructure
- **Scene Manager**  
  - `_sceneManager.h` → Scene stack, pause overlays, scene transitions, level loading, global light, input forwarding.

- **Menu Scene**  
  - `_menuScene.h` → Main Menu & Pause Menu, selectable 3D menu blocks, bullet-based menu interaction, callbacks for start/resume/help/quit.

---

### **Josh –**
*(Awaiting contribution details)*

### **Lily –**
- **UIs, Level02, Artwork**
  - **UIs**
    - Landing Page (`_landingPage.h/.cpp`, `_landingPageHandler.h/.cpp`) → Artwork/Code
    - Help Screen (`_help.h/.cpp`, `_helpMenu.h/.cpp`) → Artwork/Code
    - GameOver Screen (`_gameOverScene.h/.cpp`, `_gameOverHandler.h/.cpp`) → Artwork/Code

  - **Level02**
    - **Enemies**
      - All enemies move toward the player
      - Enemies shoot balls randomly
      - Hitboxes implemented:
        - Player touches enemy → lose a life
        - Ball touches enemy → enemy loses life
        - Enemy hits arena → stays inside
    
    - **Cube**
      - Follows the player and periodically stops
      - Blocks player shots
      - Designed to be disruptive / obstructive
      - Hitbox logic:
        - Player hits cube → lose a life
        - Ball hits cube → blocked
    
    - **Player** (Level02-specific)
      - WASD movement
      - Normal and charged shots
      - Hitbox interactions consistent with cube & enemy rules
