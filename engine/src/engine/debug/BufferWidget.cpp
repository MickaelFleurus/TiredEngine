#include "engine/debug/BufferWidget.h"

#include <string>

#include <imgui.h>

#include "engine/vulkan/BufferHandler.h"

namespace {
constexpr int kBufferCount = 5;
constexpr std::array<const char*, kBufferCount> kBufferNamesStr{
    "Vertices Buffer",      "Indexes Buffer",       "Instance Buffer",
    "Indirect Draw Buffer", "Text Instance Buffer",
};

// void DisplayVertexBufferContent(Vulkan::CBufferHandler& handler) {
//     auto& buffer = handler.GetWrapper<Core::SVertex>();
//     ImGui::Text("Buffer: %s", "Vertices Buffer");
//     auto dataVec = buffer.GetData();

//     if (ImGui::BeginTable("VertexTable", 4,
//                           ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
//         ImGui::TableSetupColumn("Index");
//         ImGui::TableSetupColumn("Position");
//         ImGui::TableSetupColumn("UV");
//         ImGui::TableSetupColumn("Normal");
//         ImGui::TableHeadersRow();

//         for (size_t i = 0; i < dataVec.size(); ++i) {
//             const auto& v = dataVec[i];
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(0);
//             ImGui::Text("%zu", i);
//             ImGui::TableSetColumnIndex(1);
//             ImGui::Text("(%.3f, %.3f, %.3f)", v.position.x, v.position.y,
//                         v.position.z);
//             ImGui::TableSetColumnIndex(2);
//             ImGui::Text("(%.3f, %.3f)", v.texCoord.x, v.texCoord.y);
//             ImGui::TableSetColumnIndex(3);
//             ImGui::Text("(%.3f, %.3f, %.3f)", v.normal.x, v.normal.y,
//                         v.normal.z);
//         }
//         ImGui::EndTable();
//     }
// }

// void DisplayIndexBufferContent(Vulkan::CBufferHandler& handler) {
//     auto& indexBuffer = handler.GetWrapper<Core::IndexType>();
//     auto& vertexBuffer = handler.GetWrapper<Core::SVertex>();
//     ImGui::Text("Buffer: %s", "Indices Buffer");
//     auto indexData = indexBuffer.GetData();
//     auto vertexData = vertexBuffer.GetData();

//     if (ImGui::BeginTable("IndexTable", 2,
//                           ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
//         ImGui::TableSetupColumn("Index");
//         ImGui::TableSetupColumn("Vertex Position");
//         ImGui::TableHeadersRow();

//         for (size_t i = 0; i < indexData.size(); ++i) {
//             Core::IndexType idx = indexData[i];
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(0);
//             ImGui::Text("%zu (%d)", i, idx);
//             ImGui::TableSetColumnIndex(1);
//             if (idx < vertexData.size()) {
//                 const auto& vertex = vertexData[idx];
//                 ImGui::Text("(%.3f, %.3f, %.3f)", vertex.position.x,
//                             vertex.position.y, vertex.position.z);
//             } else {
//                 ImGui::Text("Out of bounds");
//             }
//         }
//         ImGui::EndTable();
//     }
// }

// void DisplayInstanceBufferContent(Vulkan::CBufferHandler& handler) {
//     auto& instanceBuffer = handler.GetWrapper<Core::SInstanceData>();
//     ImGui::Text("Buffer: %s", "Instance Buffer");
//     auto instanceData = instanceBuffer.GetData();

//     if (ImGui::BeginTable("InstanceTable", 4,
//                           ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
//         ImGui::TableSetupColumn("Index");
//         ImGui::TableSetupColumn("Color");
//         ImGui::TableSetupColumn("Material ID");
//         ImGui::TableSetupColumn("Texture ID");
//         ImGui::TableHeadersRow();

//         for (size_t i = 0; i < instanceData.size(); ++i) {
//             const auto& v = instanceData[i];
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(0);
//             ImGui::Text("%zu", i);
//             ImGui::TableSetColumnIndex(1);
//             ImGui::ColorButton(
//                 ("Color##" + std::to_string(i)).c_str(),
//                 ImVec4(v.color.r, v.color.g, v.color.b, v.color.a));
//             ImGui::TableSetColumnIndex(2);
//             ImGui::Text("%d", v.materialId);
//             ImGui::TableSetColumnIndex(3);
//             ImGui::Text("%d", v.textureId);
//         }
//         ImGui::EndTable();
//     }

//     for (size_t i = 0; i < instanceData.size(); ++i) {
//         const auto& v = instanceData[i];
//         ImGui::Text("Instance %zu Model Matrix:", i);
//         if (ImGui::BeginTable(("ModelMatrixTable" +
//         std::to_string(i)).c_str(),
//                               4, ImGuiTableFlags_Borders)) {
//             for (int row = 0; row < 4; ++row) {
//                 ImGui::TableNextRow();
//                 for (int col = 0; col < 4; ++col) {
//                     ImGui::TableSetColumnIndex(col);
//                     ImGui::Text("%.3f", v.modelMatrix[row][col]);
//                 }
//             }
//             ImGui::EndTable();
//         }
//     }
// }

// void DisplayIndirectBufferContent(Vulkan::CBufferHandler& handler) {
//     auto& buffer = handler.GetWrapper<Core::SIndirectDrawCommand>();
//     ImGui::Text("Buffer: %s", "Indirect Draw Buffer");
//     auto data = buffer.GetData();

//     if (ImGui::BeginTable("IndirectTable", 6,
//                           ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
//         ImGui::TableSetupColumn("Index");
//         ImGui::TableSetupColumn("Index Count");
//         ImGui::TableSetupColumn("Instance Count");
//         ImGui::TableSetupColumn("First Index");
//         ImGui::TableSetupColumn("Vertex Offset");
//         ImGui::TableSetupColumn("First Instance");
//         ImGui::TableHeadersRow();

//         for (size_t i = 0; i < data.size(); ++i) {
//             const auto& v = data[i];
//             ImGui::TableNextRow();
//             ImGui::TableSetColumnIndex(0);
//             ImGui::Text("%zu", i);
//             ImGui::TableSetColumnIndex(1);
//             ImGui::Text("%d", v.indexCount);
//             ImGui::TableSetColumnIndex(2);
//             ImGui::Text("%d", v.instanceCount);
//             ImGui::TableSetColumnIndex(3);
//             ImGui::Text("%d", v.firstIndex);
//             ImGui::TableSetColumnIndex(4);
//             ImGui::Text("%d", v.vertexOffset);
//             ImGui::TableSetColumnIndex(5);
//             ImGui::Text("%d", v.firstInstance);
//         }
//         ImGui::EndTable();
//     }
// }

// void DisplayTextInstanceBufferContent(Vulkan::CBufferHandler& handler) {
//     auto& buffer = handler.GetWrapper<Core::SUIInstanceData>();
//     ImGui::Text("Buffer: %s", "Text Instance Buffer");
// }

// void DisplayBufferContent(Vulkan::CBufferHandler& handler, int index) {
//     switch (index) {
//     case 0:
//         DisplayVertexBufferContent(handler);
//         break;
//     case 1:
//         DisplayIndexBufferContent(handler);
//         break;
//     case 2:
//         DisplayInstanceBufferContent(handler);
//         break;
//     case 3:
//         DisplayIndirectBufferContent(handler);
//         break;
//     case 4:
//         DisplayTextInstanceBufferContent(handler);
//         break;
//     }
// }

} // namespace

namespace Debug {
CBufferWidget::CBufferWidget(Vulkan::CBufferHandler& bufferHandler)
    : mBufferHandler(bufferHandler), mBufferIndex(0) {
}

const char* CBufferWidget::GetName() const {
    return "Buffer Widget";
}

void CBufferWidget::Render() {
    bool isVisible = IsVisible();
    // if (ImGui::Begin("Vulkan Buffers", &isVisible)) {

    //     if (ImGui::BeginCombo("Select Buffer",
    //                           std::to_string(mBufferIndex).c_str())) {
    //         for (int i = 0; i < kBufferCount; ++i) {
    //             bool isSelected = (mBufferIndex == i);
    //             if (ImGui::Selectable(kBufferNamesStr[i], isSelected)) {
    //                 mBufferIndex = i;
    //             }
    //             if (isSelected) {
    //                 ImGui::SetItemDefaultFocus();
    //             }
    //         }
    //         ImGui::EndCombo();
    //     }
    //     DisplayBufferContent(mBufferHandler, mBufferIndex);

    //     ImGui::End();
    // }
}

} // namespace Debug