// #include "engine/debug/MeshComponentWidget.h"

// #include <imgui.h>
// #include <imgui_stdlib.h>

// #include "engine/component/MeshComponent.h"
// #include "engine/core/MeshManager.h"
// #include "engine/debug/TextureImGuiContainer.h"
// #include "engine/renderer/TextureManager.h"

// namespace Debug {
// CMeshComponentWidget::CMeshComponentWidget(
//     Component::CMeshComponent& meshComponent, Core::CMeshManager&
//     meshManager, CTextureImGuiContainer& textureContainer) :
//     mMeshComponent(meshComponent) , mMeshManager(meshManager) ,
//     mTextureContainer(textureContainer) , mColor(meshComponent.GetColor()) {
//     SetVisible(true);
// }

// void CMeshComponentWidget::Render() {
//     if (ImGui::ColorEdit4("Mesh Color", &mColor.r)) {
//         mMeshComponent.SetColor(mColor);
//     }
//     ImGui::Separator();
//     ImGui::Text("Mesh");
//     if (ImGui::BeginCombo("##MeshSelect", mCurrentMeshName.c_str())) {
//         if (ImGui::Selectable("Cube", mCurrentMeshName == "Cube")) {
//             auto* mesh = mMeshManager.CreateCube(1.0f);
//             mMeshComponent.SetMesh(mesh);
//             mCurrentMeshName = "Cube";
//         }
//         if (ImGui::Selectable("Triangle", mCurrentMeshName == "Triangle")) {
//             auto* mesh = mMeshManager.CreateTriangle();
//             mMeshComponent.SetMesh(mesh);
//             mCurrentMeshName = "Triangle";
//         }
//         ImGui::EndCombo();
//     }
//     ImGui::Separator();
//     ImGui::Text("Texture");
//     const auto& textures = mTextureContainer.Get();
//     if (mCurrentTextureName.empty()) {
//         const auto id = mMeshComponent.GetTextureIndex();
//         const auto it =
//             std::find_if(textures.cbegin(), textures.cend(),
//                          [id](const auto& info) { return info.id == id; });
//         mCurrentTextureName = it->name;
//         mCurrentTextureId = std::distance(textures.cbegin(), it);
//     }
//     if (ImGui::BeginCombo("##TextureSelect", mCurrentTextureName.c_str())) {

//         int i = 0;
//         for (const auto& info : textures) {
//             if (ImGui::Selectable(info.name.c_str(),
//                                   mCurrentTextureName == info.name)) {
//                 mMeshComponent.SetTextureIndex(info.id);
//                 mCurrentTextureName = info.name;
//                 mCurrentTextureId = i;
//             }
//             i++;
//         }
//         ImGui::EndCombo();
//     }
//     if (mCurrentTextureId != -1) {
//         ImGui::SameLine();
//         ImGui::Image(textures[mCurrentTextureId].imguiTexId, ImVec2(64, 64));
//     }
// }

// const char* CMeshComponentWidget::GetName() const {
//     return "Mesh Component";
// }
// } // namespace Debug
