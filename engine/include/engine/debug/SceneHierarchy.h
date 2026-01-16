#pragma once

#include <memory>
#include <optional>
#include <string>

#include "engine/core/GameObjectId.h"
#include "engine/debug/IOverlordItem.h"

namespace Scene {
class ISceneHandler;
}

namespace Core {
class CGameObject;
}

namespace Component {
class CComponentManager;
}

namespace Debug {

class CEntityWidget;

class CSceneHierarchy : public IOverlordItem {
public:
    CSceneHierarchy(Component::CComponentManager& componentManager,
                    Debug::CEntityWidget& entityWidget,
                    Scene::ISceneHandler& sceneHandler);
    ~CSceneHierarchy() override;

    void Render() override;
    const char* GetName() const override;

private:
    void DrawNodeRecursive(Core::CGameObject& obj);
    void DrawContextMenu();
    void DrawNameModal();

    Core::CGameObject* mSelectedItem{nullptr};

    Component::CComponentManager& mComponentManager;
    Debug::CEntityWidget& mEntityWidget;
    Scene::ISceneHandler& mSceneHandler;

    std::optional<std::string> mModalName{std::nullopt};
};
} // namespace Debug
