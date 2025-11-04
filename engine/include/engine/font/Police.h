#pragma once

#include "engine/font/FontTypes.h"
#include <glm/vec4.hpp>
#include <span>

namespace Material {
class AbstractMaterial;
}

namespace Font {
class CPolice {
public:
    CPolice(const char* name,
            std::unique_ptr<Material::AbstractMaterial> material,
            unsigned int size, std::span<Font::GlyphInfo> glyphs);
    ~CPolice();

    unsigned int GetSize() const;
    glm::vec4 GetColor() const;

    void SetColor(const glm::vec4& color);

    const Font::GlyphInfo& GetGlyphInfo(char c) const;

    Material::AbstractMaterial& GetMaterial();

    const std::string& GetName() const;

private:
    const std::string mName;
    std::unique_ptr<Material::AbstractMaterial> mMaterial;
    unsigned int mSize;
    std::vector<Font::GlyphInfo> mGlyphs;
};
} // namespace Font