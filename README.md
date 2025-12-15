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
  - `_level01.h/.cpp` → Major subsystem integrating all of Luis’s core systems.
  - Key responsibilities:
    - Arena mode logic (primary gameplay path).
    - Survival timer handling & level transition signaling.
    - Enemy spawning with formation presets & per-enemy randomized skins.
    - Player movement in arena-local space with clamping.
    - Obstacle collisions and damage.
    - Camera follow inside either arena or hallway systems.
    - Charge-shot aiming using cursor-to-local steering and world conversion.
    - Enemy projectile → player damage, including cooldown.
    - Player projectile → enemy kill logic.
    - Health pickup spawning, ring effect timing, and visual scaling.
#### Game Flow / Scene Infrastructure
- **Scene Manager**  
  - `_sceneManager.h` → Scene stack, pause overlays, scene transitions, level loading, global light, input forwarding.

- **Menu Scene**  
  - `_menuScene.h` → Main Menu & Pause Menu, selectable 3D menu blocks, bullet-based menu interaction, callbacks for start/resume/help/quit.

---

### **Josh –**
- **Level00**
  - Consists of three mini stages. 
  - The first stage has static targets the player must shoot down. 
  - The second stage has moving targets the player must shoot down.
  - The third stage has moving "throwers", these are enemies that throw balls forward at random intervals (not aimed towards the player)
- **Code**
  -  OBJ Loader
    - A complete OBJ file loader allowing OBJs to be converted into vertex/vertex normal/vertex texture buffers for graphical rendering
    - Supports quads/triangles + textures + normals (for lighting)
  - OBJ Animations
    - Class that supports OBJ file animations including frames, FPS, animation start/stop/reset, etc
  - Bounding Boxes
    - Class that allows for a given model to hold multiple bounding boxes
    - Allow for simple boolean collisions (mainly used for ball hit detection) or face detection (6 faces total for box)
    - Used for collision physics and ball bouncing physics
  - Sound Helper
    - Modifed existing sound class to support IrrKlang 3D spacial audio
    - Added volume+pitch adjustments to sound class
- **Other**
  - Blender
    - Made player animations + model -- exported as OBJ files
    - Made UFO model
  - CAD
    - Made map design in CAD -- exported as OBJ files
  - Sounds
    - Sound effects including: playerHit, playerDeath, targetHit, throwerHit, throwerDeath all self recorded and edited.
    - Existing sound effects modified: levelTransition, level music (clipped, fadeIn) effects added. 

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
