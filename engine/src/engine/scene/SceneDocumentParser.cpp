#include "engine/scene/SceneDocumentParser.h"

#include <fstream>
#include <unordered_set>

#include "engine/utils/Logger.h"

namespace Scene {
namespace {
template <std::size_t Size>
bool ReadArray(const nlohmann::json& value, const std::string& fieldName,
               std::array<double, Size>& result) {
    if (!value.is_array() || value.size() != Size) {
        LOG_ERROR("{} must contain {} numbers", fieldName, Size);
        return false;
    }

    for (std::size_t index = 0; index < Size; ++index) {
        if (!value[index].is_number()) {
            LOG_ERROR("{} must contain only numbers", fieldName);
            return false;
        }
        result[index] = value[index].get<double>();
    }
    return true;
}

} // namespace

SSceneDocument CSceneDocumentParser::Parse(const nlohmann::json& data) {
    try {
        if (!data.is_object()) {
            LOG_ERROR("Scene document must be an object");
            return {};
        }
        if (!data.contains("sceneName") || !data.at("sceneName").is_string()) {
            LOG_ERROR("sceneName must be a string");
            return {};
        }
        if (!data.contains("version") ||
            !data.at("version").is_number_integer()) {
            LOG_ERROR("version must be an integer");
            return {};
        }
        if (!data.contains("entities") || !data.at("entities").is_array()) {
            LOG_ERROR("entities must be an array");
            return {};
        }

        SSceneDocument document{};
        document.name = data.at("sceneName").get<std::string>();
        document.version = data.at("version").get<int>();
        if (document.name.empty() || document.version < 1) {
            LOG_ERROR("sceneName must be non-empty and version positive");
            return {};
        }

        std::unordered_set<std::string> entityNames;
        for (const auto& entityData : data.at("entities")) {
            if (!entityData.is_object() || !entityData.contains("name") ||
                !entityData.at("name").is_string()) {
                LOG_ERROR("Each entity must have a string name");
                return {};
            }

            SEntityDefinition entity;
            entity.name = entityData.at("name").get<std::string>();
            if (entity.name.empty() ||
                !entityNames.insert(entity.name).second) {
                LOG_ERROR("Entity names must be non-empty and unique");
                return {};
            }
            if (entityData.contains("hierarchy")) {
                if (!entityData.at("hierarchy").is_object()) {
                    LOG_ERROR("Entity hierarchy must be an object");
                    return {};
                }
                entity.hierarchy = entityData.at("hierarchy");
            }
            if (entityData.contains("transform")) {
                entity.transform = entityData.at("transform");
            }
            if (entityData.contains("components")) {
                const auto& components = entityData.at("components");
                if (!components.is_object()) {
                    LOG_ERROR("Entity components must be an object");
                    return {};
                }
                for (auto it = components.begin(); it != components.end();
                     ++it) {
                    if (!it.value().is_object()) {
                        LOG_ERROR("Component data must be an object: {}",
                                  it.key());
                        return {};
                    }
                    entity.components.emplace(it.key(), it.value());
                }
            }
            document.entities.push_back(std::move(entity));
        }

        return document;
    } catch (const nlohmann::json::exception& exception) {
        LOG_ERROR("Failed to read scene document: {}", exception.what());
        return {};
    }
}

SSceneDocument CSceneDocumentParser::Load(const std::filesystem::path& path) {
    std::ifstream stream(path);
    if (!stream) {
        LOG_ERROR("Failed to open scene file: {}", path.string());
        return {};
    }
    try {
        return Parse(nlohmann::json::parse(stream));
    } catch (const nlohmann::json::exception& exception) {
        LOG_ERROR("Failed to parse scene file {}: {}", path.string(),
                  exception.what());
        return {};
    }
}
} // namespace Scene
