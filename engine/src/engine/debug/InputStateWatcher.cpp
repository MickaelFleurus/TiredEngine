#include "engine/debug/InputStateWatcher.h"

#include <imgui.h>
#include <magic_enum/magic_enum.hpp>

#include "engine/debug/Overlord.h"
#include "engine/input/InputTypes.h"


namespace Debug {

CInputStateWatcher::CInputStateWatcher() {
    COverlord::AddWidget(*this, mToken);
}

const char* CInputStateWatcher::GetName() const {
    return "Input State Watcher";
}

void CInputStateWatcher::Render() {

    ImGui::Begin("Input State Watcher");

    ImGui::Text("Tracked Buttons:");
    ImGui::Separator();
    if (ImGui::BeginTable("KeyboardTable", 2,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {

        for (auto button : magic_enum::enum_values<Input::EButton>()) {
            const char* name = magic_enum::enum_name(button).data();

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%s", name);
            ImGui::TableNextColumn();
            ImGui::Text("%s", magic_enum::enum_name(
                                  mInputStateWatcher.getButtonState(button))
                                  .data());
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

} // namespace Debug
