# Dinosaur

A simple remake of the dinosaur game on Chrome browser.

## Dependency

You need SFML installed on system path or set `SFML_DIR` yourself in `CMakeLists.txt`. Lower CMake required version if needed.

## Update

### 1.1

- Organize main program into class.
    - Move many logic into appropriate places (into `update()` and `render()`).
        - Audio is not moved into `render()` due to logic flow (as of now, I want to avoid the delay between the logic and the audio).
- Attempt a minimal `ResourceManager` template class.
    - Not too useful during this project, but it's nice to implement.
- Move most event processing into `update()` now that I know about `sf::Keyboard::isKeyPressed()` existence.
- Fix a bug where if the player is crouching, then pause, then release the crouch, then unpause, the dinosaur will crouch until the next crouch key event.
- Score sound is now less loud and only occurs every 200 points.
- Score now increases every 0.05 seconds as opposed to 0.1 seconds.
- The game now run in fixed time step instead of variable time step.
    - This results in noticeable stutter if the logic frame is set lower than the refresh rate of the monitor.
- Reduce the height of `Cactus1Low` from `-25` to `-15`.
- Update header guard.

### 1.0

- Initial commit, check [this](https://youtu.be/xEKnjFSxH_0) video.
    - Minimal graphic.
    - Include sound.
    - Include pause indication.
    - Change speed incremental.
    - Change gravity to make the object less floaty.
