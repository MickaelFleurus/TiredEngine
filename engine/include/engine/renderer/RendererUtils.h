#pragma once

#include <SDL3/SDL_pixels.h>
#include <glm/mat4x4.hpp>
#include <vulkan/vulkan.h>

namespace Renderer {

VkSampler CreateSampler(VkDevice device);

VkFormat SDLPixelFormatToVulkanFormat(SDL_PixelFormat sdlFormat);

} // namespace Renderer
