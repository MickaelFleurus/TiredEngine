#include <core/GameLoop.h>
#include <engine/system/System.h>
#include <iostream>

int main(int argc, char** argv) {
    System::CSystem system;
    if (!system.Initialize()) {
        std::cout << "Failed to initialize system\n";
        return -1;
    }
    Core::CGameLoop game(system);
    if (const auto initExpected = game.Initialize(); !initExpected) {
        std::cout << "Failed to initialize: " << initExpected.error() << "\n";
        return -1;
    }

    return game.Run();
}