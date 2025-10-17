#include "debug/Overlord.h"
#include "debug/IOverlordItem.h"

#include "renderer/Window.h"

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlgpu3.h"

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gpu.h>

namespace Debug {
CGuardedContainer<IOverlordItem> COverlord::m_Items;

COverlord::COverlord(SDL_Window& window, SDL_GPUDevice& device)
    : m_Window(window), m_Device(device) {

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    [[maybe_unused]] ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForSDLGPU(&m_Window);
    ImGui_ImplSDLGPU3_InitInfo info = {};
    info.Device = &m_Device;
    info.ColorTargetFormat =
        SDL_GetGPUSwapchainTextureFormat(&m_Device, &m_Window);
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
    ImGui::Begin("My First Window");

    ImGui::Text("Hello, ImGui!");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    if (ImGui::Button("Click Me!")) {
        // Button was clicked
        printf("Button clicked!\n");
    }

    static float f = 0.0f;
    ImGui::SliderFloat("Float Slider", &f, 0.0f, 1.0f);

    static bool show_demo = false;
    ImGui::Checkbox("Show Demo Window", &show_demo);

    if (show_demo) {
        ImGui::ShowDemoWindow(&show_demo);
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplSDLGPU3_PrepareDrawData(ImGui::GetDrawData(), &cmd);
}

void COverlord::Render(SDL_GPUCommandBuffer& cmd, SDL_GPURenderPass& pass) {

    ImGui_ImplSDLGPU3_RenderDrawData(ImGui::GetDrawData(), &cmd, &pass);
}

void COverlord::AddMenu(IOverlordItem& item, CToken& token) {
    m_Items.Add(item, token);
}

void COverlord::HandleEvent(const SDL_Event* e) {
    ImGui_ImplSDL3_ProcessEvent(e);
}

} // namespace Debug