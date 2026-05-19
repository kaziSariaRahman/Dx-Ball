# DX-Ball (DX Ball)

## About

DX-Ball is a classic brick-breaker game implemented in C using OpenGL/GLUT. This project (main.cpp) implements paddle-and-ball gameplay with multiple brick types, power-up drops, shooting, and a simple HUD with high-score tracking.

## Key Features

- Classic brick-breaker gameplay (paddle, ball, bricks)
- Multiple brick types: colored (1-hit) and grey/wall bricks (3-hit with HP shown)
- Power-up / drop system (7 drop types)
- Shoot mechanic (requires SHOOT perk)
- Fireball and Through modes that alter collision behavior
- Bullets, HUD (score, lives, best), pause/menu/high-score screens
- Simple, dependency-light single-source demo (`main.cpp`)

## Gameplay Overview

- Break colored bricks to score points and progress.
- Grey (wall) bricks require 3 hits and show remaining HP.
- Bricks may drop perks when destroyed; catch them with the paddle to gain effects.
- The ball speeds up slightly every 5 bricks destroyed.
- Win by destroying all colored bricks; lose when you run out of lives.

## How to Play

1. Start the game from the main menu (`1` key).
2. Move the paddle with the mouse or arrow keys to position under the ball.
3. Launch the ball with `Space` or left-click. The ball moves automatically after launch.
4. Bounce the ball off the paddle to hit and break bricks. Aim hits toward weaker spots or the edges to change angle.
5. Catch falling drops with the paddle to gain perks (or avoid harmful drops like `DIE!` and `SHRK`).
6. Use `Z` to shoot when you have the `SHOT` perk to quickly destroy bricks above.
7. Keep an eye on the HUD for lives, score, and active power-up timers. Restart with `R` or return to the menu with `M`.

Tips:
- Aim for edges of the paddle to send the ball at sharper angles.
- Prioritize catching `+LIF` and `SHOT` perks; avoid `DIE!` drops.
- The ball speeds up as you destroy bricks—stay focused and reposition early.
- Use `P` to pause if you need a break.

## Controls

- Move paddle: Mouse move or Left/Right Arrow keys (also `A`/`D`)
- Launch ball: `Space` or Left Click
- Shoot bullet: `Z` (requires `SHOT` perk)
- Pause / Resume: `P`
- Menu / Back: `M`
- Restart (from pause/gameover/win): `R`
- Exit: `Esc`

In-menu controls: press `1` to Start, `2` for High Score, `3` for Help, `4` to Exit.

## Drops / Power-ups (caught when falling onto paddle)

1. FIRE  — Fireball: ball destroys bricks on contact (no bounce) (`FIREBALL!`)
2. THRU  — Through: ball passes through bricks (`THROUGH BRICK!`)
3. DIE!  — Instant life lost if caught (`DEATH! -1 Life`)
4. SHRK  — Shrink paddle width (`SHRUNK!`)
5. SHOT  — Enable shooting bullets with `Z` (`SHOOT! Press Z`)
6. +LIF  — Gain an extra life (`+1 LIFE!`)
7. EXP   — Expand paddle width (`EXPANDED!`)

Power-up durations: FIRE and THRU last for limited time (shown in HUD).

## Scoring & Mechanics

- Colored bricks award points; bricks in top rows give more points.
- Score increases are applied immediately and best score is tracked during the session.
- Ball increases speed every 5 bricks destroyed.

## Build & Run (Windows)

This project uses GLUT/freeglut and OpenGL. Typical build options:

- Code::Blocks / Visual Studio: open the provided project files and link against freeglut/GL/GLU.
- MinGW (example compile):

```bash
gcc main.cpp -o dxball -lfreeglut -lopengl32 -lglu32 -static
```

On Linux/macOS use `-lglut -lGL -lGLU` (or your platform's GLUT library). Adjust include/library paths as needed.

Run the game from the build output folder (for example, `bin/Debug` if using IDE build configurations).

## Assets

- This implementation draws all graphics procedurally (no external assets required).
- If you add textures or shaders later, place them in an `assets/` folder and update source paths.

## Troubleshooting

- Missing GLUT/freeglut headers or libraries: install the development package for your platform and ensure include/lib paths are set.
- Window opens but nothing visible: confirm OpenGL context creation succeeded and you're running the executable from the correct working directory.
- Controls not responding: ensure the window has focus and try both mouse and keyboard controls.

## Contributing

- Suggestions, bug reports or improvements: open an issue or fork and send a pull request.
- Keep changes minimal and document behavioral changes in your PR.

## Credits

- Original implementation in `main.cpp` by the project authors.
- Uses OpenGL and GLUT/freeglut for rendering and input.

## License

Add a `LICENSE` file to the repository and choose a license (for example MIT or Apache-2.0).

## Contact

For questions, open an issue in the repository or contact the maintainer.
