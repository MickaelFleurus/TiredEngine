#include <core/GameLoop.h>
#include <iostream>

int main(int argc, char** argv) {
    Core::CGameLoop game;
    if (const auto initExpected = game.EverythingInitialisedCorrectly();
        !initExpected) {
        std::cout << "Failed to initialize: " << initExpected.error() << "\n";
        return -1;
    }

    return game.Run();
}