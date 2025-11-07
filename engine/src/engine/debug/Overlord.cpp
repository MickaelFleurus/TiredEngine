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
CGuardedContainer<IOverlordItem> COverlord::mWidgets;
CGuardedContainer<IOverlordItem> COverlord::mMenus;

COverlord::COverlord(const Renderer::CWindow& window) : mWindow(window) {
}

void COverlord::Initialize() {

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    [[maybe_unused]] ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForSDLGPU(mWindow.Get());
    ImGui_ImplSDLGPU3_InitInfo info = {};
    info.Device = mWindow.GetDevice();
    info.ColorTargetFormat =
        SDL_GetGPUSwapchainTextureFormat(mWindow.GetDevice(), mWindow.Get());
    ImGui_ImplSDLGPU3_Init(&info);
}

COverlord::~COverlord() {
    ImGui_ImplSDLGPU3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

void COverlord::PrepareRender(SDL_GPUCommandBuffer* cmd) {
    ImGui_ImplSDLGPU3_NewFrame();
    ImGui_ImplSDL3_NewFrame();

    ImGui::NewFrame();

    RenderMenuBar();
    RenderWidgets();
    ImGui::Render();
    ImGui_ImplSDLGPU3_PrepareDrawData(ImGui::GetDrawData(), cmd);
}

void COverlord::Render(SDL_GPUCommandBuffer* cmd, SDL_GPURenderPass* pass) {
    ImGui_ImplSDLGPU3_RenderDrawData(ImGui::GetDrawData(), cmd, pass);
}

void COverlord::AddWidget(IOverlordItem& item, CToken& token) {
    mWidgets.Add(item, token);
}

void COverlord::AddMenu(IOverlordItem& item, CToken& token) {
    mMenus.Add(item, token);
}

void COverlord::HandleEvent(const SDL_Event* e) {
    ImGui_ImplSDL3_ProcessEvent(e);
}

void COverlord::RenderMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        for (auto& menu : mMenus) {
            if (menu.IsVisible()) {
                menu.Render();
            }
        }

        if (ImGui::BeginMenu("Widgets")) {
            for (auto& widget : mWidgets) {
                if (ImGui::MenuItem(widget.GetName(), nullptr,
                                    widget.IsVisible())) {
                    widget.ToggleVisible();
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void COverlord::RenderWidgets() {
    for (auto& item : mWidgets) {
        if (item.IsVisible()) {
            item.Render();
        }
    }
}

} // namespace Debug