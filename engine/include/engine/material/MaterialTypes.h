#pragma once
#include <vector>
#include <vulkan/vulkan.h>

namespace Material {
enum class EMaterialType {
    Unlit,    // Simple texture, no lighting
    Lit,      // Standard PBR lighting
    Text,     // Font rendering
    Sprite,   // 2D sprite rendering
    Particle, // Particle systems
    UI        // UI elements
};

struct SMaterialBindingInfo {
    std::vector<VkBuffer> bufferBindings;
    std::vector<VkDescriptorBufferInfo> uniformBuffers;
    std::vector<VkDescriptorImageInfo> imageViews;
};

} // namespace Material