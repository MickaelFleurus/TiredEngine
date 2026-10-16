#pragma once

#include <array>
#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>

namespace Scene {
struct SSceneTransform {
    std::array<double, 3> position{0.0f, 0.0f, 0.0f};
    std::array<double, 4> rotation{0.0f, 0.0f, 0.0f, 1.0f};
    std::array<double, 3> scale{1.0f, 1.0f, 1.0f};
};

struct SEntityDefinition {
    std::string name;
    std::optional<nlohmann::json> hierarchy;
    nlohmann::json transform;
    std::unordered_map<std::string, nlohmann::json> components;
};

struct SSceneDocument {
    std::string name;
    int version;
    std::vector<SEntityDefinition> entities;
};

class CSceneDocumentParser {
public:
    static SSceneDocument Parse(const nlohmann::json& data);
    static SSceneDocument Load(const std::filesystem::path& path);
};
} // namespace Scene
