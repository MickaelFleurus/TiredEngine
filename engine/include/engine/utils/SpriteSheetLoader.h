#pragma once

#include <string>
#include <unordered_map>

#include "engine/core/Sprite.h"

namespace Renderer {
class CTextureManager;
} // namespace Renderer

class CAssetParser;
namespace Utils {
class CFileHandler;
std::unordered_map<std::string, Core::SSpriteInfo> LoadSpriteSheet(
    const CAssetParser& assetParser, const CFileHandler& fileHandler,
    Renderer::CTextureManager& textureManager, const std::string& name);

} // namespace Utils
