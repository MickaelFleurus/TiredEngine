#include "engine/font/FontHandler.h"

#include "engine/material/AbstractMaterial.h"
#include "engine/material/MaterialFactory.h"
#include "engine/renderer/TextureManager.h"
#include "engine/utils/FileHandler.h"
#include <SDL3/SDL_surface.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace {

constexpr unsigned int kStartChar =
    32; // Space. Everything before is non-printable
constexpr unsigned int kEndChar = 126; // Tilde. Last printable ASCII character
constexpr unsigned int kMargin = 8;    // Margin around each glyph in atlas

nlohmann::json
GlyphsToJson(const std::array<Font::GlyphInfo, kEndChar - kStartChar>& glyphs) {
    nlohmann::json jsonData;
    unsigned char c = kStartChar;
    for (const auto& info : glyphs) {
        nlohmann::json glyphJson;
        glyphJson["uv"] = {{"x", info.uv.x},
                           {"y", info.uv.y},
                           {"w", info.uv.z},
                           {"h", info.uv.w}};
        glyphJson["advance"] = info.advance;
        glyphJson["offset"] = {{"x", info.offset.x}, {"y", info.offset.y}};
        glyphJson["size"] = {{"x", info.size.x}, {"y", info.size.y}};
        jsonData[std::string(1, c++)] = glyphJson;
    }
    return jsonData;
}

std::vector<Font::GlyphInfo> JsonToGlyphs(const nlohmann::json& jsonData) {
    std::vector<Font::GlyphInfo> glyphs;
    for (char c = kStartChar; c < kEndChar; ++c) {
        const auto& glyphJson = jsonData[std::string(1, c)];
        Font::GlyphInfo info;
        info.uv.x = glyphJson["uv"]["x"];
        info.uv.y = glyphJson["uv"]["y"];
        info.uv.z = glyphJson["uv"]["w"];
        info.uv.w = glyphJson["uv"]["h"];
        info.advance = glyphJson["advance"];
        info.offset.x = glyphJson["offset"]["x"];
        info.offset.y = glyphJson["offset"]["y"];
        info.size.x = glyphJson["size"]["x"];
        info.size.y = glyphJson["size"]["y"];
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
    std::array<Font::GlyphInfo, kEndChar - kStartChar> glyphs;
    int cx = 0, cy = 0;
    for (unsigned int c = kStartChar; c < kEndChar; ++c) {
        SDL_Surface* glyph = TTF_RenderGlyph_Solid(font, c, white);
        if (!glyph) {
            continue;
        }

        const int glyphIndex = c - kStartChar;
        Font::GlyphInfo& glyphInfo = glyphs[glyphIndex];
        SDL_Rect dst = {cx * glyphSize, cy * glyphSize, glyph->w, glyph->h};
        SDL_BlitSurface(glyph, NULL, atlasSurface, &dst);

        glm::vec4 uv = {static_cast<float>(dst.x), static_cast<float>(dst.y),
                        static_cast<float>(dst.w), static_cast<float>(dst.h)};
        uv.x /= atlasSurface->w;
        uv.y /= atlasSurface->h;
        uv.z /= atlasSurface->w;
        uv.w /= atlasSurface->h;
        glyphInfo.uv = uv;
        glyphInfo.size = {static_cast<float>(glyph->w),
                          static_cast<float>(glyph->h)};

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

namespace Font {
CFontHandler::CFontHandler(Renderer::CTextureManager& textureManager,
                           Utils::CFileHandler& fileHandler,
                           Material::CMaterialFactory& materialFactory)
    : mTextureManager(textureManager)
    , mFileHandler(fileHandler)
    , mMaterialFactory(materialFactory) {
    TTF_Init();
}

CFontHandler::~CFontHandler() {
    TTF_Quit();
}

CPolice& CFontHandler::GetPolice(const char* name, unsigned int size) {
    SFont key{name, size};
    auto it = mPolices.find(key);
    if (it != mPolices.end()) {
        return it->second;
    }

    const std::string glyphTexFilePath = std::format(
        "{}/font_textures/{}_{}", mFileHandler.GetTempFolder(), name, size);
    SDL_Surface* surface = nullptr;
    std::vector<Font::GlyphInfo> glyphs;
    if (mFileHandler.DoesFileExist(glyphTexFilePath, ".bmp")) {
        surface = mFileHandler.LoadTextureFileBMP(glyphTexFilePath);
        glyphs = JsonToGlyphs(mFileHandler.LoadJson(glyphTexFilePath));
    } else {
        auto fontPath = std::format("{}/fonts/{}.ttf",
                                    mFileHandler.GetAssetsFolder(), name);
        TTF_Font* font =
            TTF_OpenFont(fontPath.c_str(), static_cast<float>(size));
        auto [loadedSurface, loadedGlyphs] = CreateFontData(size, font);
        surface = loadedSurface;
        glyphs = std::vector<Font::GlyphInfo>(loadedGlyphs.begin(),
                                              loadedGlyphs.end());
        mFileHandler.SaveTextureFileBMP(glyphTexFilePath, surface);
        mFileHandler.SaveJson(glyphTexFilePath, GlyphsToJson(loadedGlyphs));
        TTF_CloseFont(font);
    }
    mTextureManager.LoadTextureFromSurface(name, surface);
    SDL_DestroySurface(surface);

    auto [emplaced_it, inserted] = mPolices.try_emplace(
        key, name, std::move(mMaterialFactory.CreateTextMaterial(name)), size,
        glyphs);
    return emplaced_it->second;
}

} // namespace Font