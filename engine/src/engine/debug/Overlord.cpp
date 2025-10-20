#include "engine/debug/Overlord.h"
#include "engine/debug/IOverlordItem.h"

#include "engine/renderer/Window.h"

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlgpu3.h>

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gpu.h>

#include <magic_enum/magic_enum.hpp>

namespace Debug {
CGuardedContainer<IOverlordItem> COverlord::mItems;

COverlord::COverlord(SDL_Window& window, SDL_GPUDevice& device)
    : mWindow(window), mDevice(device) {

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    [[maybe_unused]] ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForSDLGPU(&mWindow);
    ImGui_ImplSDLGPU3_InitInfo info = {};
    info.Device = &mDevice;
    info.ColorTargetFormat =
        SDL_GetGPUSwapchainTextureFormat(&mDevice, &mWindow);
    ImGui_ImplSDLGPU3_Init(&info);
}

COverlord::~COverlord() {
    ImGui_ImplSDLGPU3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

void COverlord::PrepareRender(SDL_GPUCommandBuffer& cmd) {
    ImGui_ImplSDLGPU3_NewFrame();
    ImGui_ImplSDL3_NewFrame();

    ImGui::NewFrame();

    ShowMenuBar();
    for (auto& item : mItems) {
        if (item.IsVisible()) {
            item.Render();
        }
    }
    ImGui::Render();
    ImGui_ImplSDLGPU3_PrepareDrawData(ImGui::GetDrawData(), &cmd);
}

void COverlord::Render(SDL_GPUCommandBuffer& cmd, SDL_GPURenderPass& pass) {
    ImGui_ImplSDLGPU3_RenderDrawData(ImGui::GetDrawData(), &cmd, &pass);
}

void COverlord::AddMenu(IOverlordItem& item, CToken& token) {
    mItems.Add(item, token);
}

void COverlord::HandleEvent(const SDL_Event* e) {
    ImGui_ImplSDL3_ProcessEvent(e);
}

void COverlord::ShowMenuBar() {
    if (ImGui::BeginMainMenuBar()) {

        if (ImGui::BeginMenu("Scenes")) {
            // for (auto& sceneName :
            //      magic_enum::enum_values<Scene::ESceneType>()) {

            //     const char* name = magic_enum::enum_name(sceneName).data();
            //     if (ImGui::MenuItem(name)) {
            //     }
            // }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Widgets")) {
            for (auto& item : mItems) {
                if (ImGui::MenuItem(item.GetName(), nullptr,
                                    item.IsVisible())) {
                    item.ToggleVisible();
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

} // namespace Debug