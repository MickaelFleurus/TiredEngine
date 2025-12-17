#pragma once
#include "engine/utils/Hashing.h"
#include <string>

namespace Font {

struct SFont {
    std::string fontName;
    char character;

    auto operator<=>(const SFont&) const = default;
};

struct SFontHash {
    std::size_t operator()(const SFont& key) const {
        return Utils::CreateHash(key.fontName, key.character);
    }
};

} // namespace Font