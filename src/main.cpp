#include <SDL3/SDL.h>
#include <core/GameLoop.h>
#include <engine/system/System.h>
#include <engine/vulkan/VulkanContext.h>
#include <engine/vulkan/VulkanInitializer.h>
#include <iostream>

SDL_Window* CreateSDLWindow(const System::CSystem& system) {
    const std::string& gameName = system.GetGameName();
    const System::SDisplayParameter& displayParams =
        system.GetDisplayParameters();

    SDL_WindowFlags windowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN;
    if (displayParams.fullscreen) {
        windowFlags |= SDL_WINDOW_FULLSCREEN;
    }
    auto* window = SDL_CreateWindow(gameName.c_str(), displayParams.width,
                                    displayParams.height, windowFlags);
    return window;
}

int main(int argc, char** argv) {
    System::CSystem system;
    if (!system.Initialize()) {
        std::cout << "Failed to initialize system\n";
        return -1;
    }

    SDL_Window* window = CreateSDLWindow(system);
    if (!window) {
        std::cout << "Failed to create SDL window\n";
        return -1;
    }

    SDL_InitSubSystem(SDL_INIT_VIDEO);
    Vulkan::CVulkanContext vulkanContext{window};
    Vulkan::InitializeVulkan(vulkanContext, window, system);
    vulkanContext.RecreateSwapchainResources();

    Core::CGameLoop game(system, window, vulkanContext);

    game.Run();
    SDL_DestroyWindow(window);
    return 0;
}