#pragma once

struct SDL_Window;

namespace System {
class CSystem;
} // namespace System

namespace Vulkan {
class IVulkanContextSetter;
void InitializeVulkan(IVulkanContextSetter& context, SDL_Window* window,
                      const System::CSystem& system);
} // namespace Vulkan