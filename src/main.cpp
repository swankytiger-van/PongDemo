#include "Game.h"
#include <filesystem>
#include <iostream>
int main()
{

    std::cout << "Current working dir: "
        << std::filesystem::current_path().string() << '\n';
    Game game(800, 600, "PongCore Iter1");
    game.run();
    return 0;
}
