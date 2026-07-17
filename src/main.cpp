#include <iostream>

#include <SDL3/SDL.h>
#include <core/GameLoop.h>
#include <engine/system/System.h>
#include <engine/utils/AssetParser.h>
#include <engine/vulkan/VulkanContext.h>
#include <engine/vulkan/VulkanInitializer.h>

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
    SDL_SetWindowSurfaceVSync(window, 0);
    return window;
}

int main(int argc, char** argv) {
    System::CSystem system;
    if (!system.Initialize()) {
        std::cout << "Failed to initialize system\n";
        return -1;
    }

    std::unique_ptr<SDL_Window, void (*)(SDL_Window*)> window =
        std::unique_ptr<SDL_Window, void (*)(SDL_Window*)>(
            CreateSDLWindow(system), SDL_DestroyWindow);
    if (!window) {
        std::cout << "Failed to create SDL window\n";
        return -1;
    }

    SDL_InitSubSystem(SDL_INIT_VIDEO);

    Vulkan::SContext context =
        Vulkan::InitializeVulkan(std::move(window), system);
    Vulkan::CSwapchain swapchain(context);
    swapchain.Recreate();

    Core::CGameLoop game(system, context, swapchain);

    game.Run();
    return 0;
}
