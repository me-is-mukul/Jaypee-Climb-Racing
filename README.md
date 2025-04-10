# Car Dodge Game

## Overview
"Car Dodge Game" is a fast-paced 2D arcade game built using Raylib in C++. Players control a car to dodge enemies, collect coins, and use power-ups like shields, magnets, and nitro boosts. The game features a start menu, settings, leaderboard, and particle effects for an engaging experience.

**Developers**: Mukul, Ananya, Aastha  
**Version**: Ultimate Edition  
**Date**: April 10, 2025

## Features
- **Dynamic Gameplay**: Dodge enemies, collect coins, and activate power-ups.
- **Power-Ups**: Shield (10s), Magnet (10s, 800px radius), Nitro (5s).
- **Customizable**: Change car skins in settings.
- **Leaderboard**: Save top 5 scores with player names.
- **Audio**: Background music and sound effects (coin, crash, etc.).
- **Visuals**: Scrolling road, particle explosions.

## Requirements
- **Compiler**: g++ (MinGW for Windows)
- **Library**: Raylib (installed at `C:/raylib/raylib/src`)
- **Assets**: Place all `.png` and `.wav` files in an `assets/` folder.

## Setup Instructions
1. **Install Raylib**:
   - Download Raylib and extract to `C:/raylib/`.
   - Ensure `raylib/src/` contains the library files.
2. **Clone/Download**:
   - Get the source code (`main.cpp`) and assets.
3. **Compile**:
   ```bash
   g++ main.cpp -o main.exe -LC:/raylib/raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm
   ```
4. **Run**:
   - Execute `main.exe` from the directory with `assets/`.

## Gameplay
- **Objective**: Avoid enemies, collect coins, and survive as long as possible.
- **Start**: Press ENTER from the main menu to begin.
- **Settings**: Adjust volume and car skin (S from menu).
- **Leaderboard**: View top scores (L from menu).
- **Game Over**: Enter your name to save your score (R to restart).

## Controls
- **Movement**: 
  - LEFT/RIGHT: Move car horizontally.
  - UP: Accelerate.
- **Menu**:
  - ENTER: Start game.
  - S: Settings.
  - L: Leaderboard.
  - Q: Quit.
  - ESC: Back to menu (from Settings/Leaderboard).
- **Game**:
  - P: Pause/unpause.
  - R: Restart after game over.
- **Settings**:
  - +/-: Adjust volume.
  - LEFT/RIGHT: Change car skin.

## File Structure
- `main.cpp`: Source code.
- `assets/`:
  - `road.png`: Background.
  - `player1.png`, `player2.png`, `player3.png`: Car skins.
  - `enemy1.png`, `enemy2.png`, `enemy3.png`: Enemy cars.
  - `coin.png`, `shield.png`, `magnet.png`, `nitro.png`: Items.
  - `coin.wav`, `crash.wav`, `engine.wav`, `powerup.wav`, `background.mp3`: Audio.
- `leaderboard.txt`: Saved scores (generated).

## Developer Notes
- **Window Centering**: Set to (100, 100) manually; adjust via `SetWindowPosition()` for true centering.
- **Magnet Radius**: 800px, tweakable in `CheckCollisionCircleRec()` in `main()`.
- **Power-Up Durations**: Shield (10s), Magnet (10s), Nitro (5s) in `Player::ActivatePowerUp()`.
- **Debugging**: Check asset paths if textures/sounds fail to load.

## Credits
- **Raylib**: Graphics and audio library.
- **Team**: Mukul, Ananya, Aastha for design and coding.

---

### Usage
1. Save this as `README.md` in your project folder.
2. Adjust paths (e.g., `C:/raylib/`) or asset names if they differ in your setup.
3. Use it to document your project for others or yourself!

Let me know if you want to tweak it further!
