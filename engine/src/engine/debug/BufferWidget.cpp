#include "engine/debug/BufferWidget.h"

#include <imgui.h>

#include "engine/vulkan/BufferHandler.h"

namespace Debug {
CBufferWidget::CBufferWidget(Vulkan::CBufferHandler& bufferHandler)
    : mBufferHandler(bufferHandler) {
}

const char* CBufferWidget::GetName() const {
    return "Buffer Widget";
}

void CBufferWidget::Render() {
    if (!ImGui::Begin("Vulkan Buffers", &mVisible)) {
        ImGui::End();
        return;
    }

    const auto buffers = mBufferHandler.GetBufferSizes();
    ImGui::Text("Configured capacity: %zu buffers", buffers.size());
    if (ImGui::BeginTable("BufferCapacities", 2,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Buffer");
        ImGui::TableSetupColumn("Capacity");
        ImGui::TableHeadersRow();

        for (const auto& [name, size] : buffers) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(name.c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.2f MiB", static_cast<double>(size) /
                                         (1024.0 * 1024.0));
        }
        ImGui::EndTable();
    }
    ImGui::End();
}

} // namespace Debug