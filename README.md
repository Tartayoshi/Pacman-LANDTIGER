# Pacman for LandTiger Borad

*Classic Pacman port for LandTiger board written `C`*

![Pac Man](https://www.customcontrollers.com/cdn/shop/articles/Untitled_-_2025-04-30T111838.925_f657f1cf-c1a6-40c0-b5ea-2503357193b2.png?v=1747385518&width=1400)

[Wikipedia Link to Pacman](https://it.wikipedia.org/wiki/Pac-Man)

>## WARNING
>The LCD_GetPoint function don't always work as expected,
is highly dependent on the screen model of the board. Some issues has been addressed modifing
GLCD library, but I'm not sure that I had every possible screen - board combination to try it on.
>
>So if any glitches are present on the screen, try using a diffrent board/screen combination!

- [Pacman for LandTiger Borad](#pacman-for-landtiger-borad)
- [Specifications](#specifications)
- [Demo Run (Video)](#demo-run-video)
- [Implementation](#implementation)
    - [State Machine \& Game Loop](#state-machine--game-loop)
    - [Rendering Engine](#rendering-engine)
    - [Input Handling](#input-handling)
    - [Artificial Intelligence (Ghost)](#artificial-intelligence-ghost)
    - [Sound Engine](#sound-engine)
- [Installation](#installation)


# Specifications
The project had to follow thight specifcation that made it look the way it is.

To mention the most important ones:
- Labyrinth with **240 pills** and **6 Power Pills** spawned in a random position at a random time
- Pacman move with the board **joystick**. It follow the direction until a new directions is provided as input or it hit a wall
- Pacman should be able to **teleport** when exiting the screen
- Pills give **10 points**, Power pills **50**
- Every **1000 points** Pacman receive a **new life**
- **INT0** Pause the game: Place a “PAUSE” message in the middle of the screen and stop Pacman
movement. Pressing again **INT0** resume the game (delete the Pause message and resume
movement). The game starts in “PAUSE” mode.
- The ghost use **A\* algorithm**
- The Game has **sounds effects**

---

# Demo Run (Video)

<!-- <video width="1200" height="480" controls>
  <source src="/docs/videos/V2_Gameplay.mp4" type="video/mp4">
</video> -->

[![Watch Explenation on YouTube](https://img.youtube.com/vi/cxqU0tUidUw/maxresdefault.jpg)](https://youtu.be/cxqU0tUidUw)

# Implementation

The game is built around a state machine architecture, driven by hardware interrupts for timing, input, and sound. The core design choices are outlined below:

<!-- <video width="1200" height="480" controls>
  <source src="/docs/videos/V1_Explenation.mp4" type="video/mp4">
</video> -->

[![Watch Demo on YouTube](https://img.youtube.com/vi/4j-t7Of7gTc/hqdefault.jpg)](https://youtu.be/4j-t7Of7gTc)

### State Machine & Game Loop
The main game logic is managed by a state machine within `gameEngine.c`, cycling through states like `LOADING`, `READY`, `INGAME`, `PAUSE`, and `GAMEOVER`.

- **Interrupt-Driven Core**: Instead of a busy-wait loop, the `main()` function puts the processor in a low-power "Wait For Interrupt" (`wfi`) state.
- **Game Tick**: A 25ms `TIMER3` interrupt is responsible for calling the main `update()` function. This function acts as the primary "game tick," advancing the game state and handling entity logic.
- **Input & Sound Loop**: A separate 50ms Repetitive Interrupt Timer (`RIT`) handles polling the joystick and buttons, and also drives the sound engine. This decouples the time-critical game logic from slower I/O operations.
- **Game Clock**: A 1Hz `TIMER0` interrupt serves as the real-time clock for the game's countdown timer.

### Rendering Engine
The visual presentation is handled by a combination of a basic graphics library and direct bitmap rendering.

- **Graphics Library**: The `GLCD` library provides fundamental drawing functions for text and simple shapes.
- **Bitmap-Based Sprites**: All game entities (Pacman, ghosts, pills, map walls) are rendered as bitmaps. The raw image data is stored in `bitmapData.c` and drawn to the screen using functions from `bitmap.c`.
- **Manual Frame Buffering**: The engine does not use a double buffer. To create movement, the game manually saves the screen area behind a sprite before drawing it. On the next frame, this saved background is restored, and the sprite is drawn in its new position. This approach is memory-efficient but susceptible to visual artifacts, as noted by the warnings about the `LCD_GetPoint` function.
- **Chromakey Optimization**: To accelerate rendering, especially for the map and pills, the `BITMAP_draw` function implements a chromakey technique. It avoids drawing pixels of a specific color (Magenta, used as the alpha/transparent color), skipping the time-consuming process of rendering the black background for every tile and making the initial draw and any map redraws almost instantaneous.

### Input Handling
Input is processed through a combination of interrupts and a polling mechanism.

- **Polling within an ISR**: The `RIT_IRQHandler` polls the GPIO pins connected to the joystick to determine its current direction. The state of the pause button (INT0) is also managed here.
- **Buffered Commands**: The player's requested direction is stored in a volatile global variable. The `inputHandler` in the main game logic processes this variable, allowing the next move to be "buffered" until Pacman reaches an intersection in the grid.

### Artificial Intelligence (Ghost)
The ghost's behavior is state-driven, utilizing the A* pathfinding algorithm for intelligent movement.

- **A\* Pathfinding**: In its `CHASING` state, the ghost uses the A\* algorithm (`aStar` in `ghost.c`) to calculate the most efficient path to the player's current position on the map grid.
- **State-Based Behavior**: The ghost operates in several states:
    - `CHASING`: Actively hunts the player using A\*.
    - `FLEEING`: Triggered when Pacman eats a Power Pill. The AI is reversed: the ghost identifies all possible moves at an intersection and randomly chooses one that is *not* on the shortest path to Pacman, creating a convincing "panic" behavior.
    - `DEAD`: After being eaten, the ghost uses A\* to navigate back to its spawn point.
    - `SPAWNING`: A simple, scripted movement to exit the spawn area.

### Sound Engine
The sound system is integrated directly into the 50ms `RIT` interrupt handler.

- **Event-Driven Audio**: The handler checks the current game state and event flags (e.g., `pacmanDeath`, `ghostDeath`, `eatingPills`) to determine which sound to play.
- **Single-Channel Player**: It uses a simple, blocking note player (`music.c`) that can only play one sound at a time. A priority system is implemented within the ISR to decide which sound takes precedence (e.g., earning a life is a higher priority than the standard "waka-waka" sound).

---

# Installation
To run the project you need to open the project with [KEIL]() environment setted up for LPC1768 LANDTIGER board.

While it can ran also on simulators it might run slowed or in an incorrect way.
