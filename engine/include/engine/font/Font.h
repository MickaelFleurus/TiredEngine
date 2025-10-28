#pragma once
#include <bitset>
#include <compare>
#include <string>

namespace Renderer {

struct SFont {
    std::string fontPath;
    unsigned int fontSize;

    auto operator<=>(const SFont&) const = default;
};

struct SFontHash {
    std::size_t operator()(const SFont& key) const {
        return std::hash<std::string>{}(key.fontPath) ^
               (std::hash<unsigned int>{}(key.fontSize) << 1);
    }
};

} // namespace Renderer