#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vulkan/vulkan.h>

#include "engine/renderer/RendererUtils.h"

namespace Renderer {

// Per-character instance data
struct SCharacterInstance {
    glm::vec2 position{0.0f};
    glm::vec2 size{1.0f};
    glm::vec4 uvRect{0.0f};
    glm::vec4 color{1.0f};
};

class CTextRenderer {
public:
    explicit CTextRenderer(VkDevice device,
                           VkPhysicalDeviceMemoryProperties memProperties);

    void Initialize();

    VkBuffer GetQuadVertexBuffer() const;
    VkBuffer GetQuadIndexBuffer() const;

    VulkanBuffer CreateInstanceBuffer(size_t maxCharacters);

    void UpdateInstanceBuffer(VulkanBuffer& buffer,
                              const std::vector<SCharacterInstance>& instances);

private:
    VkDevice mDevice = VK_NULL_HANDLE;
    VkPhysicalDeviceMemoryProperties mMemProperties{};
    Renderer::VulkanBuffer mQuadVertexBuffer{};
    Renderer::VulkanBuffer mQuadIndexBuffer{};
};

} // namespace Renderer