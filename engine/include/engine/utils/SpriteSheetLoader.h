#pragma once

#include <string>
#include <unordered_map>

#include "engine/core/Sprite.h"

namespace Renderer {
class CTextureManager;
} // namespace Renderer

namespace Utils {
class CFileHandler;

std::unordered_map<std::string, Core::SSpriteInfo>
LoadSpriteSheet(Utils::CFileHandler& fileHandler,
                Renderer::CTextureManager& textureManager,
                const std::string& name);

} // namespace Utils
