#include "debug/Overlord.h"
#include "debug/IOverlordItem.h"

#include "renderer/Window.h"

#include <imgui.h>

namespace Debug {
CGuardedContainer<IOverlordItem> COverlord::m_Items;

COverlord::COverlord(Renderer::CWindow& window) : m_Window(window) {

    /*IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();*/
}

void COverlord::Render() {
    /*ImGui::Begin("Overlord Debug Menu");
    for (auto& item : m_Items) {
        item.Render();
    }
    ImGui::End();*/
}

void COverlord::AddMenu(IOverlordItem& item, CToken& token) {
    m_Items.Add(item, token);
}

} // namespace Debug