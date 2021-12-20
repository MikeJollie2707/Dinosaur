#include "game.hpp"
#include <iostream>

int main()
{
    try {
        Game game(1.0 / 75.0);

        std::cout << "Objective: Don't get hit by the cacti (green boxes) or birds (orange boxes).\n";
        std::cout << "Controls:\n";
        std::cout << "- Space/Arrow Up: Jump (hold to ascend higher)\n";
        std::cout << "- Arrow Down: Crouch (hold to keep crouching)\n";
        std::cout << "- Esc: Toggle pause\n";
        std::cout << "The game is pausing; press Esc to play.\n";

        game.run();
    }
    catch (std::exception& e) {
        std::cout << e.what() << '\n';
    }

    return 0;
}
