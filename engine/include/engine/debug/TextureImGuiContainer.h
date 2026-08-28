// #pragma once
// #include <vector>

// #include <imgui.h>

// namespace Renderer {
// class CTextureManager;
// struct VulkanTexture;
// } // namespace Renderer

// namespace Debug {
// class CTextureImGuiContainer {
// public:
//     struct SImGuiTextureInfo {
//         std::string name;
//         int id;
//         const Renderer::VulkanTexture& vkInfo;
//         ImTextureID imguiTexId;
//     };
//     CTextureImGuiContainer(Renderer::CTextureManager& textureManager);
//     ~CTextureImGuiContainer();

//     const std::vector<SImGuiTextureInfo>& Get();

// private:
//     Renderer::CTextureManager& mTextureManager;
//     std::vector<SImGuiTextureInfo> mImGuiTextureIds;
// };
// } // namespace Debug
