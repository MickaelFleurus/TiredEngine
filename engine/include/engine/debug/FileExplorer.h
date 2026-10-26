#pragma once

#include <cstddef>
#include <expected>
#include <filesystem>
#include <span>
#include <string>
#include <vector>

struct SDL_Window;
namespace Debug {

enum EFileType { Image, Sound, SpriteAnimations, Scenes };

using ExpectedFile = std::expected<std::filesystem::path, std::string>;
using ExpectedFiles =
    std::expected<std::vector<std::filesystem::path>, std::string>;

ExpectedFile SelectFile(std::span<EFileType> types, SDL_Window* window);
ExpectedFiles SelectFiles(std::span<EFileType> types, SDL_Window* window);
ExpectedFile SaveFile(SDL_Window* window);

} // namespace Debug
