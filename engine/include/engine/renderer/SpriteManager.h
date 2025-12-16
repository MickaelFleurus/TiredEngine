#pragma once
#include

namespace Renderer {

class CSpriteManager {
public:
    CSpriteManager();
    ~CSpriteManager();

    void LoadSpriteSheet(const std::string& spriteSheetName);

private:
    std::unordered_map<std::string, Core::SSpriteInfo> mSprites;
};
} // namespace Renderer