# Game (SFML starter)

This folder contains a minimal SFML-based starter project and concise SFML notes to get the game going.

Quick summary
- Uses SFML via CMake FetchContent (SFML 3.0.1).
- Example executable: `game` (built from `src/main.cpp`).
- `src/main.cpp` shows a basic render window, event loop, dt timing (sf::Clock), and drawing.

Build & run (WSL / Linux)
```bash
cd /mnt/c/Game-Project/Game
mkdir -p build && cd build
cmake -G "Ninja" ..    # or cmake .. if you prefer Makefiles
cmake --build . --config Release
./bin/game
```

Build & run (Windows PowerShell)
```powershell
Set-Location "C:\Game-Project\Game"
mkdir build; Set-Location build
# Use Ninja if installed, otherwise Visual Studio generator
cmake -G "Ninja" ..
cmake --build . --config Release
.\bin\game.exe
```

Notes & mini-reference (key SFML concepts used)
- Window & rendering: use `sf::RenderWindow`, call `pollEvent` in a loop, use `clear` → `draw` → `display`.
- Timing: `sf::Clock` and `sf::Time` for dt; use `clock.restart()` each frame.
- Input: handle events for discrete input (KeyPressed), use `sf::Keyboard::isKeyPressed` for realtime checks.
- Resources: textures/fonts should be kept alive for as long as sprites/text use them.
- Views: `sf::View` acts like a camera — set on the window with `window.setView(view)`.

If you want, next I can:
- Add a small resource manager (texture/font cache).
- Add a simple Entity/Scene hierarchy (scene graph helper) and demo movement.
- Wire keyboard movement and camera (view) following the player.

Tell me which of the above you'd like next and I'll implement it inside this `Game` folder.

To run - wsl bash -lc "cd /mnt/c/Game-Project/Game && cmake -S . -B build -G 'Ninja' && cmake --build build --config Release"