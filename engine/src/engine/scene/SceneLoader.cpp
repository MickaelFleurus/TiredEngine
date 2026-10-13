#include "engine/scene/SceneLoader.h"

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
    for (const auto& entityJson : loaded.entities) {
        auto entityObj =
            mEntityManager.Create(CStringId(entityJson.name.c_str()));
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
} // namespace Scene
