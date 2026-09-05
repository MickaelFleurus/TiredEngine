#include "engine/font/FontHandler.h"

#include <SDL3/SDL_surface.h>
#include <msdf-atlas-gen/msdf-atlas-gen.h>

#include "engine/material/AbstractMaterial.h"
#include "engine/material/MaterialManager.h"
#include "engine/renderer/TextureManager.h"
#include "engine/thread/Pool.h"
#include "engine/utils/AssetParser.h"
#include "engine/utils/FileHandler.h"
#include "engine/utils/Logger.h"

namespace {

struct SFontData {
    SDL_Surface* surface = nullptr;
    std::unordered_map<std::string, Font::GlyphInfo> glyphInfos;
    msdfgen::FontMetrics fontMetrics{};
};

nlohmann::json
GlyphsToJson(std::unordered_map<std::string, Font::GlyphInfo> glyphs,
             msdfgen::FontMetrics& fontMetrics) {
    nlohmann::json jsonData;
    jsonData["glyphs"] = nlohmann::json::object();
    for (const auto it : glyphs) {
        nlohmann::json glyphJson;
        glyphJson["uvMin"] = {{"x", it.second.uvMin.x},
                              {"y", it.second.uvMin.y}};
        glyphJson["uvMax"] = {{"x", it.second.uvMax.x},
                              {"y", it.second.uvMax.y}};
        glyphJson["planeBounds"] = {{"x", it.second.planeBounds.x},
                                    {"y", it.second.planeBounds.y},
                                    {"z", it.second.planeBounds.z},
                                    {"w", it.second.planeBounds.w}};
        glyphJson["advance"] = it.second.advance;

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
        return !glyphJson.is_null() && glyphJson.contains("uvMin") &&
               glyphJson.contains("planeBounds") &&
               glyphJson.contains("uvMax") && glyphJson.contains("advance");
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
            const auto& uvMin = glyphJson["uvMin"];
            if (uvMin.contains("x") && uvMin.contains("y")) {
                info.uvMin.x = uvMin["x"];
                info.uvMin.y = uvMin["y"];
            }

            const auto& uvMax = glyphJson["uvMax"];
            if (uvMax.contains("x") && uvMax.contains("y")) {
                info.uvMax.x = uvMax["x"];
                info.uvMax.y = uvMax["y"];
            }

            const auto& size = glyphJson["planeBounds"];
            if (size.contains("x") && size.contains("y") &&
                size.contains("z") && size.contains("w ")) {
                info.planeBounds.x = size["x"];
                info.planeBounds.y = size["y"];
                info.planeBounds.z = size["z"];
                info.planeBounds.w = size["w"];
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

void LoadExistingFontData(const SAsset& asset, SFontData& fontData,
                          Utils::CFileHandler& fileHandler) {
    fontData.surface = fileHandler.LoadTextureFile(asset.mPath);
    nlohmann::json jsonData = fileHandler.LoadJson(asset.mMetadata.value());
    fontData.glyphInfos = JsonToGlyphs(jsonData);
    fontData.fontMetrics = JsonToFontMetrics(jsonData);
}

SFontData CreateFontData(msdfgen::FreetypeHandle* freetype,
                         const std::string& fontPath) {
    SFontData fontData{};

    msdfgen::FontHandle* font = msdfgen::loadFont(freetype, fontPath.c_str());
    if (!font) {
        LOG_ERROR("Failed to load font: {}", fontPath);
        return fontData;
    }

    std::vector<msdf_atlas::GlyphGeometry> glyphs;

    msdf_atlas::FontGeometry fontGeometry(&glyphs);
    fontGeometry.loadCharset(font, 1.0, msdf_atlas::Charset::ASCII);

    // Get font metrics for proper baseline alignment
    fontData.fontMetrics = fontGeometry.getMetrics();

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

    fontData.surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_RGBA32);
    if (fontData.surface) {
        SDL_LockSurface(fontData.surface);
        Uint8* dst = static_cast<Uint8*>(fontData.surface->pixels);
        int pitch = fontData.surface->pitch;
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
        SDL_UnlockSurface(fontData.surface);
    }

    for (const auto& glyph : glyphs) {
        Font::GlyphInfo info;
        double l, b, r, t;
        glyph.getQuadAtlasBounds(l, b, r, t);
        info.planeBounds = {static_cast<float>(l), static_cast<float>(b),
                            static_cast<float>(r), static_cast<float>(t)};

        glyph.getQuadPlaneBounds(l, b, r, t);
        info.uvMin = {static_cast<float>(l) / width,
                      1.0f - static_cast<float>(b) / height};
        info.uvMax = {static_cast<float>(r) / width,
                      1.0f - static_cast<float>(t) / height};

        double advance = glyph.getAdvance();
        info.advance = static_cast<float>(advance);

        std::string charKey(1,
                            static_cast<unsigned char>(glyph.getCodepoint()));
        fontData.glyphInfos.emplace(charKey, info);
    }

    // Cleanup
    msdfgen::destroyFont(font);

    return fontData;
}

} // namespace

namespace Font {
CFontHandler::CFontHandler(Utils::CFileHandler& fileHandler,
                           Thread::CPool& threadPool,
                           const CAssetParser& assetParser,
                           Renderer::CTextureManager& textureManager)
    : mFileHandler(fileHandler)
    , mThreadPool(threadPool)
    , mAssetParser(assetParser)
    , mTextureManager(textureManager) {
}

CFontHandler::~CFontHandler() {
}

std::optional<std::reference_wrapper<CPolice>>
CFontHandler::GetPolice(std::size_t id) {
    if (mPolices.size() <= id) {
        return std::nullopt;
    }
    return mPolices[id];
}

CPolice& CFontHandler::GetPolice(const std::string& name) {
    auto it = mPoliceNameToIndex.find(name);
    if (it != mPoliceNameToIndex.end()) {
        return mPolices[it->second];
    }

    auto fontTexturePath = mAssetParser.Get(EAssetType::Texture, name);
    if (!fontTexturePath || !fontTexturePath->get().mMetadata.has_value()) {
        LOG_FATAL("Font {} does not seems to exists or has not be loaded yet: "
                  "file not found",
                  name);
    }

    SFontData fontData;
    LoadExistingFontData(fontTexturePath->get(), fontData, mFileHandler);
    auto textureIndex =
        mTextureManager.LoadTextureFromSurface(name, fontData.surface);

    SDL_DestroySurface(fontData.surface);
    std::size_t index = mPolices.size();
    mPoliceNameToIndex.emplace(name, index);

    mPolices.emplace_back(
        name.c_str(), textureIndex->second, fontData.glyphInfos,
        CPolice::SMetrics{
            fontData.fontMetrics.emSize, fontData.fontMetrics.ascenderY,
            fontData.fontMetrics.descenderY, fontData.fontMetrics.lineHeight,
            fontData.fontMetrics.underlineY,
            fontData.fontMetrics.underlineThickness});
    return mPolices.back();
}

void CFontHandler::LoadAllThePolices() {
    auto freeType = msdfgen::initializeFreetype();
    if (!freeType) {
        LOG_ERROR("Failed to initialize FreeType!");
        return;
    }

    LOG_INFO("Loading fonts...");
    std::vector<std::function<void()>> jobs;
    const auto fontFiles = mAssetParser.Get(EAssetType::Font);
    for (const auto& fontFile : fontFiles) {
        const std::string glyphTexFilePath =
            std::format("{}/textures/{}.png", mFileHandler.GetTempFolder(),
                        fontFile.mPath.stem().string());
        if (!mFileHandler.DoesFileExist(glyphTexFilePath)) {
            jobs.push_back(
                [this, fontFile, freeType]() { LoadFont(fontFile, freeType); });
        }
    }
    auto group = mThreadPool.EnqueueJobs(jobs);
    group->WaitForCompletion();
    LOG_INFO("All fonts loaded.");
}

bool CFontHandler::LoadFont(const SAsset& fontAsset,
                            msdfgen::FreetypeHandle* freeType) {

    const auto fontName = fontAsset.mPath.stem().string();
    if (!freeType) {
        freeType = msdfgen::initializeFreetype();
        if (!freeType) {
            LOG_ERROR("Failed to initialize FreeType for font: {}", fontName);
            return false;
        }
    }

    SFontData fontData = CreateFontData(freeType, fontAsset.mPath.string());

    if (!mFileHandler.SaveTextureFile(fontName, fontData.surface)) {
        LOG_ERROR("Could not save font texture {}.", fontName);
        return false;
    }
    if (!mFileHandler.SaveJson(
            fontName, "textures",
            GlyphsToJson(fontData.glyphInfos, fontData.fontMetrics))) {
        LOG_ERROR("Could not save font metadata {}.", fontName);
        return false;
    }
    LOG_INFO("Loaded font: {}", fontName);
    return true;
}

} // namespace Font
