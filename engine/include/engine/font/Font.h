#pragma once
#include <bitset>
#include <compare>
#include <string>

namespace Font {

struct SFont {
    std::string fontName;
    char character;

    auto operator<=>(const SFont&) const = default;
};

struct SFontHash {
    std::size_t operator()(const SFont& key) const {
        return std::hash<std::string>{}(key.fontName) ^
               (std::hash<unsigned char>{}(key.character) << 1);
    }
};

} // namespace Font