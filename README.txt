** CONTROLS **
WASD :: Movement (Forward/Left/Backwards/Right)
Left Mouse :: Launch Dodgeball

** CONTRIBUTIONS **
Luis -
Josh - 
Lily - UIs, Level02, Artwork
  - UIs
    - Landing Page (_landingPage.h/.cpp, _landingPageHandler.h/.cpp) -> Artwork/Code
    - Help Screen (_help.h/.cpp, _helpMenu.h/.cpp) -> Artwork/Code
    - GameOver screen (_gameOverScene.h/.cpp, _gameOverHandler.h/.cpp) -> Artwork/Code
  - Level02
    - Enemies
      - All eneimes go towards the player
      - All enemies shots balls randomly.
      - Has hitbox.
        - Player touches it -> Player loses a life.
        - Ball touches it -> enemy loses life.
        - Hit's arena -> stays inside.
    - Cube
      - Cube follows the player and stops. Continues after a certain amount of time.
      - Mainly to block player from shotting balls at enemies.
      - Be annoying basicly.
      - Has hitbox.
        - Player hits cube -> loses a life.
        - Ball hits cube -> Noting just blocks it.
    - Player
      - Moves with WASD.
      - Shoots normally with Left Click.
      - Shoots a charged ball with holding Left Click.
      - Has hitbox.
        - Ball hits player -> loses life.
        - Other hit related items are mentioned in Cube and Enemeis that affect player.
