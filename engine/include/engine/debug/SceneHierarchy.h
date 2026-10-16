#pragma once

#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

#include "engine/core/Entity.h"
#include "engine/debug/IOverlordItem.h"

namespace Component {
class CManager;
}

namespace Core {
class CEntityManager;
}

namespace Scene {
class CSceneLoader;
}

namespace Debug {
struct SEntitySelection {
    std::optional<Core::SEntity> entity;
};

class CSceneHierarchy : public IOverlordItem {
public:
    CSceneHierarchy(Core::CEntityManager& entityManager,
                    Component::CManager& componentManager,
                    SEntitySelection& selection,
                    Scene::CSceneLoader& sceneLoader);

    void Render() override;
    const char* GetName() const override;

private:
    struct SEntityPayload {
        uint32_t id;
        uint32_t generation;
    };

    void RenderEntityNode(Core::SEntity entity,
                          const std::vector<Core::SEntity>& entities,
                          std::unordered_set<uint32_t>& visited);
    bool RenderEntityContextMenu(Core::SEntity entity);
    void RenderRenameModal();
    Core::SEntity CreateEntity(std::optional<Core::SEntity> parent);
    bool RenameEntity(Core::SEntity entity, const std::string& name);
    void DeleteEntity(Core::SEntity entity);
    void DeleteEntityRecursive(Core::SEntity entity,
                               std::unordered_set<uint32_t>& visited);
    bool ReparentEntity(Core::SEntity entity,
                        std::optional<Core::SEntity> parent);

    Core::CEntityManager& mEntityManager;
    Component::CManager& mComponentManager;
    SEntitySelection& mSelection;
    Scene::CSceneLoader& mSceneLoader;
    std::optional<Core::SEntity> mRenameEntity;
    std::string mRenameBuffer;
    std::string mSaveMessage;
    bool mSaveSucceeded = false;
    bool mRenameError = false;
    bool mRenamePopupRequested = false;
    bool mRenameNeedsFocus = false;
};

class CEntityInspector : public IOverlordItem {
public:
    CEntityInspector(Core::CEntityManager& entityManager,
                     Component::CManager& componentManager,
                     SEntitySelection& selection);

    void Render() override;
    const char* GetName() const override;

private:
    void RenderSelectedEntity();

    Core::CEntityManager& mEntityManager;
    Component::CManager& mComponentManager;
    SEntitySelection& mSelection;
};
} // namespace Debug
