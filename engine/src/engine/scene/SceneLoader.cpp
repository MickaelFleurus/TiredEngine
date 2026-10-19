#include "engine/scene/SceneLoader.h"

#include <algorithm>
#include <fstream>
#include <system_error>

#include "engine/component/TransformComponent.h"
#include "engine/component/UIComponents.h"
#include "engine/core/EngineLoop.h"
#include "engine/core/EntityManager.h"
#include "engine/scene/SceneDocumentParser.h"
#include "engine/utils/ComponentParserRegistry.h"
#include "engine/utils/FileHandler.h"

namespace Scene {
CSceneLoader::CSceneLoader(Core::CEngineLoop& engineLoop,
                           Utils::CFileHandler& fileHandler,
                           Core::CEntityManager& entityManager,
                           Component::CManager& componentManager)
    : mEngineLoop(engineLoop)
    , mFileHandler(fileHandler)
    , mEntityManager(entityManager)
    , mComponentManager(componentManager) {
}

void CSceneLoader::LoadSceneFromFile(const std::filesystem::path& path) {
    auto loaded = CSceneDocumentParser::Load(path);
    const auto sourceDocument = mFileHandler.LoadJson(path.string());
    if (loaded.name.empty()) {
        return;
    }

    mCurrentScenePath = path;
    mOriginalEntityData.clear();
    for (const auto& entityJson : loaded.entities) {
        auto entityObj =
            mEntityManager.Create(CStringId(entityJson.name.c_str()));
        if (sourceDocument.contains("entities") &&
            sourceDocument.at("entities").is_array()) {
            const auto original = std::ranges::find_if(
                sourceDocument.at("entities"), [&](const auto& rawEntity) {
                    return rawEntity.is_object() &&
                           rawEntity.value("name", std::string{}) ==
                               entityJson.name;
                });
            if (original != sourceDocument.at("entities").end()) {
                mOriginalEntityData[entityObj.id] = {entityObj.generation,
                                                     *original};
            }
        }
        if (entityJson.hierarchy) {
            ComponentRegistry::Apply("Hierarchy", mComponentManager, entityObj,
                                     entityJson.hierarchy);
        }
        ComponentRegistry::Apply("Transform", mComponentManager, entityObj,
                                 entityJson.transform);

        for (const auto& [componentName, componentData] :
             entityJson.components) {
            ComponentRegistry::Apply(componentName, mComponentManager,
                                     entityObj, componentData);
        }
    }
}

bool CSceneLoader::SaveCurrentScene() const {
    if (mCurrentScenePath.empty()) {
        return false;
    }

    const auto savePath =
        mFileHandler.ResolveSourceAssetPath(mCurrentScenePath);
    nlohmann::json document = mFileHandler.LoadJson(savePath.string());
    if (!document.is_object()) {
        document = nlohmann::json::object();
    }
    document["sceneName"] =
        document.value("sceneName", mCurrentScenePath.stem().string());
    document["version"] = document.value("version", 1);
    document["entities"] = nlohmann::json::array();

    auto entities = mEntityManager.GetEntities();
    std::ranges::sort(entities, {}, &Core::SEntity::id);
    auto& localTransforms =
        mComponentManager.GetPool<Component::SLocalTransform>();
    auto& hierarchies = mComponentManager.GetPool<Component::SHierarchy>();

    auto& sprites = mComponentManager.GetPool<Component::SSprite>();
    auto& texts = mComponentManager.GetPool<Component::SText>();

    for (const auto& entity : entities) {
        nlohmann::json entityData = nlohmann::json::object();
        if (const auto original = mOriginalEntityData.find(entity.id);
            original != mOriginalEntityData.end() &&
            original->second.first == entity.generation) {
            entityData = original->second.second;
        }
        entityData["name"] = entity.name.GetName();
        entityData.erase("parent");

        if (localTransforms.Has(entity)) {
            const auto& transform = localTransforms.Get(entity);
            const auto& p = transform.position;
            const auto& r = transform.rotation;
            const auto& s = transform.scale;
            entityData["transform"] = {{"pos", {p.x, p.y, p.z}},
                                       {"rot", {r.x, r.y, r.z, r.w}},
                                       {"scale", {s.x, s.y, s.z}}};
        }

        if (hierarchies.Has(entity)) {
            const auto& hierarchy = hierarchies.Get(entity);
            nlohmann::json hierarchyData;
            hierarchyData["parent"] = hierarchy.parent.GetName();
            hierarchyData["children"] = nlohmann::json::array();
            for (const auto& child : hierarchy.children) {
                hierarchyData["children"].push_back(child.GetName());
            }
            entityData["hierarchy"] = std::move(hierarchyData);
        } else {
            entityData.erase("hierarchy");
        }

        auto components =
            entityData.value("components", nlohmann::json::object());
        if (!components.is_object()) {
            components = nlohmann::json::object();
        }

        if (sprites.Has(entity)) {
            const auto& value = sprites.Get(entity);
            components["SpriteRenderer"] = {
                {"path", value.spriteName},
                {"size", {value.size.x, value.size.y}},
                {"maintainAspectRatio", value.maintainAspectRatio},
                {"color",
                 {value.color.r, value.color.g, value.color.b, value.color.a}}};
        } else {
            components.erase("SpriteRenderer");
        }

        if (texts.Has(entity)) {
            const auto& value = texts.Get(entity);
            components["Text"] = {
                {"value", value.content},
                {"fontName", value.fontName},
                {"fontSize", value.fontSize},
                {"color",
                 {value.color.r, value.color.g, value.color.b, value.color.a}},
                {"isAnimated", value.isAnimated}};
        } else {
            components.erase("Text");
        }

        if (components.empty()) {
            entityData.erase("components");
        } else {
            entityData["components"] = std::move(components);
        }
        document["entities"].push_back(std::move(entityData));
    }

    const auto temporaryPath = savePath.string() + ".tmp";
    {
        std::ofstream output(temporaryPath, std::ios::out | std::ios::trunc);
        if (!output) {
            return false;
        }
        output << document.dump(4) << '\n';
        if (!output) {
            return false;
        }
    }
    {
        std::ofstream output(mCurrentScenePath,
                             std::ios::out | std::ios::trunc);
        if (!output) {
            return false;
        }
        output << document.dump(4) << '\n';
        if (!output) {
            return false;
        }
    }

    std::error_code error;
    std::filesystem::rename(temporaryPath, savePath, error);
    if (error) {
        std::filesystem::remove(temporaryPath);
        return false;
    }
    return true;
}
} // namespace Scene
