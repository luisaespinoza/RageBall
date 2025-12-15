# Dodgeball Game

## Controls
- **W A S D** – Movement (Forward / Left / Backward / Right)
- **Left Mouse** – Launch Dodgeball

---

## Contributions

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
  - `_level01.h/.cpp` → Major subsystem integrating all of Luis’s core systems.  
  - Key responsibilities:
    - Arena mode logic (primary gameplay path)
    - Survival timer handling & level transition signaling
    - Enemy spawning with formation presets & randomized skins
    - Player movement in arena-local space with clamping
    - Obstacle collisions and damage
    - Camera follow inside arena or hallway systems
    - Charge-shot aiming via cursor-to-local conversions
    - Enemy projectile → player damage logic
    - Player projectile → enemy kill logic
    - Health pickup spawning & visual effects

---

### Game Flow / Scene Infrastructure

#### Scene Manager

The `SceneManager` is the central orchestration layer that coordinates every major subsystem of the game. It maintains a full **scene stack**, enabling gameplay scenes, overlays, menus, and pause screens to layer cleanly on top of one another, always updating and rendering in the correct order. It also handles smooth **scene transitions**, **level loading**, and **in-game menus**, ensuring that switching contexts never breaks game flow. Behind the scenes, it forwards input, applies the correct OpenGL projection, and manages global lighting to keep the engine visually consistent. It is the glue that holds the entire project together.

#### Key Responsibilities
- **Scene Stack Management**  
  Handles push/pop of scenes, pause overlays, layered UI, and safe stack mutations between frames.

- **Scene Transitions & Flow Control**  
  Provides clean transitions between gameplay, menus, help screens, game-over handlers, and level states.

- **Level Loading System**  
  Uses a level registry to construct levels, link progression, and dispatch lifecycle callbacks.

- **Input Forwarding**  
  Routes Win32 messages & scene-specific input.

- **Rendering Pipeline Setup**  
  Applies projection changes, configures OpenGL, and ensures proper bottom-to-top rendering.

- **Global Lighting & GL State**  
  Initializes depth testing, texturing, blending, and global light.

- **In-Game Menu & Overlays**  
  Preserves gameplay scenes, shows pause overlays, and restores smoothly.

- **Game Over & Reset Handling**  
  Clears scene stack, restarts levels, or boots main menu with correct callbacks.

#### Menu Scene
- `_menuScene.h` → Main Menu & Pause Menu  
  Selectable 3D menu blocks, bullet-driven menu interaction, callbacks for start/resume/help/quit.

---

### **Josh –**

#### Level00
- Three mini stages:
  - Stage 1: static targets  
  - Stage 2: moving targets  
  - Stage 3: moving “throwers” that fire balls randomly

#### Code
- **OBJ Loader**
  - Complete OBJ→buffer pipeline  
  - Supports quads/triangles, textures, normals

- **OBJ Animations**
  - Frame-based animation system (FPS, start/stop/reset)

- **Bounding Boxes**
  - Multi-box per model  
  - Boolean or face-aware collisions  
  - Used for physics + ball bounces

- **Sound Helper**
  - Enhanced IrrKlang 3D audio  
  - Volume + pitch support

#### Other
- **Blender**
  - Player animations + model  
  - UFO model

- **CAD**
  - Map design exported as OBJ

- **Audio**
  - Self-recorded SFX (playerHit, enemyHit, deaths, etc.)  
  - Edited/modified existing sounds (transitions, level music clips/fade)

---

### **Lily –**

#### UIs, Level02, Artwork

##### UIs
- **Landing Page** (`_landingPage.*`) – Artwork + Code  
- **Help Screen** (`_help.*`, `_helpMenu.*`) – Artwork + Code  
- **GameOver Screen** (`_gameOverScene.*`, `_gameOverHandler.*`) – Artwork + Code  

##### Level02

###### Enemies
- Move toward player  
- Shoot randomly  
- Hitboxes:
  - Player touches enemy → lose life  
  - Ball hits enemy → enemy loses life  
  - Enemy hits arena → clamped inside  

###### Cube
- Follows player, periodically stops  
- Blocks shots  
- Hitbox:
  - Player hits cube → lose life  
  - Ball hits cube → blocked  

###### Player (Level02-specific)
- WASD movement  
- Normal + charged shots  
- Hitbox interactions consistent with cube/enemy rules

