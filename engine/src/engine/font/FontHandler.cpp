#include "engine/font/FontHandler.h"

#include "engine/material/AbstractMaterial.h"
#include "engine/material/MaterialFactory.h"
#include "engine/renderer/TextureManager.h"
#include "engine/utils/FileHandler.h"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_surface.h>
#include <msdf-atlas-gen/msdf-atlas-gen.h>

namespace {
nlohmann::json
GlyphsToJson(std::unordered_map<std::string, Font::GlyphInfo> glyphs,
             msdfgen::FontMetrics& fontMetrics) {
    nlohmann::json jsonData;
    jsonData["glyphs"] = nlohmann::json::object();
    for (const auto it : glyphs) {
        nlohmann::json glyphJson;
        glyphJson["uv"] = {{"x", it.second.uv.x},
                           {"y", it.second.uv.y},
                           {"w", it.second.uv.z},
                           {"h", it.second.uv.w}};
        glyphJson["advance"] = it.second.advance;
        glyphJson["offset"] = {{"x", it.second.offset.x},
                               {"y", it.second.offset.y}};
        glyphJson["size"] = {{"x", it.second.size.x}, {"y", it.second.size.y}};
        jsonData["glyphs"][it.first] = glyphJson;
    }
    jsonData["fontMetrics"] = {
        {"emSize", fontMetrics.emSize},
        {"ascenderY", fontMetrics.ascenderY},
        {"descenderY", fontMetrics.descenderY},
        {"lineHeight", fontMetrics.lineHeight},
        {"underlineY", fontMetrics.underlineY},
        {"underlineThickness", fontMetrics.underlineThickness}};
    return jsonData;
}

std::unordered_map<std::string, Font::GlyphInfo>
JsonToGlyphs(const nlohmann::json& jsonData) {
    constexpr auto validateGlyphJson = [](const nlohmann::json& glyphJson) {
        return !glyphJson.is_null() && glyphJson.contains("uv") &&
               glyphJson.contains("advance") && glyphJson.contains("offset") &&
               glyphJson.contains("size");
    };
    std::unordered_map<std::string, Font::GlyphInfo> glyphs;
    if (jsonData.contains("glyphs")) {
        for (auto it : jsonData["glyphs"].items()) {
            const auto& glyphJson = it.value();
            if (!validateGlyphJson(glyphJson)) {
                continue;
            }
            Font::GlyphInfo info;

            info.advance = glyphJson["advance"];
            const auto& uv = glyphJson["uv"];
            if (uv.contains("x") && uv.contains("y") && uv.contains("w") &&
                uv.contains("h")) {
                info.uv.x = uv["x"];
                info.uv.y = uv["y"];
                info.uv.z = uv["w"];
                info.uv.w = uv["h"];
            }

            const auto& offset = glyphJson["offset"];
            if (offset.contains("x") && offset.contains("y")) {
                info.offset.x = offset["x"];
                info.offset.y = offset["y"];
            }

            const auto& size = glyphJson["size"];
            if (offset.contains("x") && offset.contains("y")) {
                info.size.x = size["x"];
                info.size.y = size["y"];
            }
            glyphs.emplace(it.key(), info);
        }
    }
    return glyphs;
}

msdfgen::FontMetrics JsonToFontMetrics(const nlohmann::json& jsonData) {
    msdfgen::FontMetrics fontMetrics;
    fontMetrics.emSize = jsonData["fontMetrics"]["emSize"];
    fontMetrics.ascenderY = jsonData["fontMetrics"]["ascenderY"];
    fontMetrics.descenderY = jsonData["fontMetrics"]["descenderY"];
    fontMetrics.lineHeight = jsonData["fontMetrics"]["lineHeight"];
    fontMetrics.underlineY = jsonData["fontMetrics"]["underlineY"];
    fontMetrics.underlineThickness =
        jsonData["fontMetrics"]["underlineThickness"];
    return fontMetrics;
}

auto CreateFontData(const std::string& fontPath) {
    std::unordered_map<std::string, Font::GlyphInfo> glyphInfos;
    SDL_Surface* surface = nullptr;
    msdfgen::FontMetrics fontMetrics{};

    auto freeType = msdfgen::initializeFreetype();
    if (!freeType) {
        return std::make_tuple(surface, glyphInfos, fontMetrics);
    }
    msdfgen::FontHandle* font = msdfgen::loadFont(freeType, fontPath.c_str());
    if (!font) {
        msdfgen::deinitializeFreetype(freeType);
        return std::make_tuple(surface, glyphInfos, fontMetrics);
    }

    std::vector<msdf_atlas::GlyphGeometry> glyphs;

    msdf_atlas::FontGeometry fontGeometry(&glyphs);
    fontGeometry.loadCharset(font, 1.0, msdf_atlas::Charset::ASCII);

    // Get font metrics for proper baseline alignment
    fontMetrics = fontGeometry.getMetrics();

    constexpr double maxCornerAngle = 3.0;
    for (auto& glyph : glyphs) {
        glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, maxCornerAngle, 0);
    }

    msdf_atlas::TightAtlasPacker packer;
    packer.setDimensionsConstraint(
        msdf_atlas::DimensionsConstraint::POWER_OF_TWO_SQUARE);
    packer.setMinimumScale(24.0);
    packer.setPixelRange(2.0);
    packer.setMiterLimit(1.0);

    packer.pack(glyphs.data(), glyphs.size());

    int width = 0, height = 0;
    packer.getDimensions(width, height);

    msdf_atlas::ImmediateAtlasGenerator<
        float, 3, msdf_atlas::msdfGenerator,
        msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, 3>>
        generator(width, height);

    msdf_atlas::GeneratorAttributes attributes;
    generator.setAttributes(attributes);
    generator.generate(glyphs.data(), glyphs.size());

    auto bitmapRef = static_cast<msdfgen::BitmapConstRef<msdf_atlas::byte, 3>>(
        generator.atlasStorage());

    surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA32);
    if (surface) {
        SDL_LockSurface(surface);
        Uint8* dst = static_cast<Uint8*>(surface->pixels);
        int pitch = surface->pitch;
        const msdf_atlas::byte* src = bitmapRef.pixels;

        for (int y = 0; y < height; ++y) {
            Uint8* row = dst + y * pitch;
            const msdf_atlas::byte* srcRow = src + y * width * 3;
            for (int x = 0; x < width; ++x) {
                row[x * 4 + 0] = srcRow[x * 3 + 0]; // R
                row[x * 4 + 1] = srcRow[x * 3 + 1]; // G
                row[x * 4 + 2] = srcRow[x * 3 + 2]; // B
                row[x * 4 + 3] = 255;               // A
            }
        }
        SDL_UnlockSurface(surface);
    }

    const double atlasScale = packer.getScale();

    for (const auto& glyph : glyphs) {
        Font::GlyphInfo info;
        double l, b, r, t;
        glyph.getQuadAtlasBounds(l, b, r, t);
        l /= width;
        r /= width;
        b /= height;
        t /= height;

        // UV coordinates (normalized to 0-1)
        info.uv.x = static_cast<float>(l);
        info.uv.y = static_cast<float>(b);
        info.uv.z = static_cast<float>(r - l);
        info.uv.w = static_cast<float>(t - b);

        // Glyph metrics
        double advance = glyph.getAdvance();
        info.advance = static_cast<float>(advance);

        glyph.getQuadPlaneBounds(l, b, r, t);
        info.offset.x = static_cast<float>(l);
        info.offset.y = static_cast<float>(b);
        info.size.x = static_cast<float>(r - l);
        info.size.y = static_cast<float>(t - b);

        std::string charKey(1,
                            static_cast<unsigned char>(glyph.getCodepoint()));
        glyphInfos.emplace(charKey, info);
    }

    // Cleanup
    msdfgen::destroyFont(font);
    msdfgen::deinitializeFreetype(freeType);

    return std::make_tuple(surface, glyphInfos, fontMetrics);
}

} // namespace

namespace Font {
CFontHandler::CFontHandler(Renderer::CTextureManager& textureManager,
                           Utils::CFileHandler& fileHandler,
                           Material::CMaterialFactory& materialFactory)
    : mTextureManager(textureManager)
    , mFileHandler(fileHandler)
    , mMaterialFactory(materialFactory) {
}

CFontHandler::~CFontHandler() {
}

CPolice& CFontHandler::GetPolice(const char* name, unsigned int size) {
    SFont key{name, size};
    auto it = mPolices.find(key);
    if (it != mPolices.end()) {
        return it->second;
    }

    const std::string glyphTexFilePath =
        std::format("{}/font_textures/{}", mFileHandler.GetTempFolder(), name);
    SDL_Surface* surface = nullptr;
    std::unordered_map<std::string, Font::GlyphInfo> glyphs;
    msdfgen::FontMetrics fontMetrics{};
    if (mFileHandler.DoesFileExist(glyphTexFilePath, ".bmp")) {
        surface = mFileHandler.LoadTextureFileBMP(glyphTexFilePath);
        auto jsonData = mFileHandler.LoadJson(glyphTexFilePath);
        glyphs = JsonToGlyphs(jsonData);
        fontMetrics = JsonToFontMetrics(jsonData);
    } else {
        auto [loadedSurface, loadedGlyphs, metrics] =
            CreateFontData(std::format("{}/fonts/{}.ttf",
                                       mFileHandler.GetAssetsFolder(), name));
        surface = loadedSurface;
        glyphs = std::move(loadedGlyphs);
        fontMetrics = metrics;
        mFileHandler.SaveTextureFileBMP(glyphTexFilePath, surface);
        mFileHandler.SaveJson(glyphTexFilePath,
                              GlyphsToJson(glyphs, fontMetrics));
    }

    mTextureManager.LoadTextureFromSurface(name, surface);
    SDL_DestroySurface(surface);

    auto [emplaced_it, inserted] = mPolices.try_emplace(
        key, name, std::move(mMaterialFactory.CreateTextMaterial(name)), size,
        glyphs,
        CPolice::SMetrics{fontMetrics.ascenderY, fontMetrics.descenderY,
                          fontMetrics.lineHeight, fontMetrics.underlineY,
                          fontMetrics.underlineThickness});
    return emplaced_it->second;
}

} // namespace Font