#include "engine/font/FontHandler.h"

#include "engine/utils/FileHandler.h"
#include <SDL3/SDL_surface.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace {

constexpr unsigned int kStartChar =
    32; // Space. Everything before is non-printable
constexpr unsigned int kEndChar = 126; // Tilde. Last printable ASCII character
constexpr unsigned int kMargin = 8;    // Margin around each glyph in atlas

nlohmann::json GlyphsToJson(const std::array<Font::GlyphInfo, 126>& glyphs) {
    nlohmann::json jsonData;
    char c = kStartChar;
    for (const auto& info : glyphs) {
        nlohmann::json glyphJson;
        glyphJson["rect"] = {{"x", info.rect.x},
                             {"y", info.rect.y},
                             {"w", info.rect.w},
                             {"h", info.rect.h}};
        glyphJson["advance"] = info.advance;
        glyphJson["offset"] = {{"x", info.offset.x}, {"y", info.offset.y}};
        jsonData[c++] = glyphJson;
    }
    return jsonData;
}

std::vector<Font::GlyphInfo> JsonToGlyphs(const nlohmann::json& jsonData) {
    std::vector<Font::GlyphInfo> glyphs;
    for (char c = kStartChar; c <= kEndChar; ++c) {
        const auto& glyphJson = jsonData[std::string(1, c)];
        Font::GlyphInfo info;
        info.rect.x = glyphJson["rect"]["x"];
        info.rect.y = glyphJson["rect"]["y"];
        info.rect.w = glyphJson["rect"]["w"];
        info.rect.h = glyphJson["rect"]["h"];
        info.advance = glyphJson["advance"];
        info.offset.x = glyphJson["offset"]["x"];
        info.offset.y = glyphJson["offset"]["y"];
        glyphs.push_back(info);
    }
    return glyphs;
}

auto CreateFontData(unsigned int fontSize, TTF_Font* font) {

    const int glyphSize = fontSize + kMargin;
    const int cols = 16;
    const int rows = ((kEndChar - kStartChar + 1) + cols - 1) / cols;

    SDL_Surface* atlasSurface = SDL_CreateSurface(
        cols * glyphSize, rows * glyphSize, SDL_PIXELFORMAT_RGBA32);

    SDL_FillSurfaceRect(
        atlasSurface, nullptr,
        SDL_MapRGBA(SDL_GetPixelFormatDetails(atlasSurface->format), nullptr, 0,
                    0, 0, 0));

    SDL_Color white = {255, 255, 255, 255};
    std::array<Font::GlyphInfo, 126> glyphs;
    int cx = 0, cy = 0;
    for (unsigned int c = kStartChar; c <= kEndChar; ++c) {
        SDL_Surface* glyph = TTF_RenderGlyph_Solid(font, c, white);
        if (!glyph) {
            continue;
        }

        const int glyphIndex = c - kStartChar;
        Font::GlyphInfo& glyphInfo = glyphs[c - kStartChar];
        SDL_Rect dst = {cx * glyphSize, cy * glyphSize, glyph->w, glyph->h};
        SDL_BlitSurface(glyph, NULL, atlasSurface, &dst);

        // store rect info for UVs later
        SDL_Rect rect = dst;
        glyphInfo.rect = rect;

        TTF_GetGlyphMetrics(font, c, NULL, NULL, NULL, NULL,
                            &glyphInfo.advance);

        SDL_DestroySurface(glyph);

        if (++cx >= cols) {
            cx = 0;
            cy++;
        }
    }

    return std::make_pair(atlasSurface, glyphs);
}

} // namespace

namespace Renderer {
CFontHandler::CFontHandler(SDL_GPUDevice* device,
                           Utils::CFileHandler& fileHandler)
    : mDevice(device), mFileHandler(fileHandler) {
    TTF_Init();
}

CFontHandler::~CFontHandler() {
    for (auto& [key, police] : mPolices) {
        SDL_ReleaseGPUTexture(mDevice, police.GetAtlas());
    }
    TTF_Quit();
}

CPolice& CFontHandler::GetPolice(const char* name, unsigned int size) {
    SFont key{name, size};
    auto it = mPolices.find(key);
    if (it != mPolices.end()) {
        return it->second;
    }

    SDL_Surface* surface = nullptr;
    std::vector<Font::GlyphInfo> glyphs;
    if (mFileHandler.DoesTextureExist(name)) {
        surface = mFileHandler.LoadTexture(name);
        glyphs = JsonToGlyphs(mFileHandler.LoadJson(name));
    } else {
        TTF_Font* font = TTF_OpenFont(name, size);
        auto [loadedSurface, loadedGlyphs] = CreateFontData(size, font);
        surface = loadedSurface;
        glyphs = std::vector<Font::GlyphInfo>(loadedGlyphs.begin(),
                                              loadedGlyphs.end());
        mFileHandler.SaveTexture(name, surface);
        mFileHandler.SaveJson(name, GlyphsToJson(loadedGlyphs));
        TTF_CloseFont(font);
    }

    // Upload to GPU
    SDL_GPUTextureCreateInfo texInfo = {
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
        .width = static_cast<Uint32>(surface->w),
        .height = static_cast<Uint32>(surface->h)};
    auto texture = SDL_CreateGPUTexture(mDevice, &texInfo);
    SDL_DestroySurface(surface);

    auto emplaced = mPolices.emplace(key, CPolice{texture, size, glyphs});
    return emplaced.first->second;
}

} // namespace Renderer