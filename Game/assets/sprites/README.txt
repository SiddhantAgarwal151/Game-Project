Place your sprite sheets here.

Expected for player:
- `player.png` — a small spritesheet with rows = directions (down, left, right, up) and columns = frames.
- Default frame size in Player is 32x32 and frames per row = 3. If you use different sizes, update Player::load call in `main.cpp`.

Tip: Export from Aseprite as a single horizontal or grid spritesheet.
wsl bash -lc "cd /mnt/c/Game-Project/Game && cmake --build build && ./build/bin/game"