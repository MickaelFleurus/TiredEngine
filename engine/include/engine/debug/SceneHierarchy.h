#pragma once

#include <memory>
#include <optional>

#include "engine/core/GameObjectId.h"
#include "engine/debug/IOverlordItem.h"

namespace Scene {
class ISceneHandler;
}

namespace Core {
class CGameObject;
}

namespace Debug {

class CEntityWidget;

class CSceneHierarchy : public IOverlordItem {
public:
    CSceneHierarchy(Debug::CEntityWidget& entityWidget,
                    Scene::ISceneHandler& sceneHandler);
    ~CSceneHierarchy() override;

    void Render() override;
    const char* GetName() const override;

private:
    void DrawNodeRecursive(Core::CGameObject& obj);
    void DrawContextMenu();
    std::optional<Core::CGameObject*> mSelectedItem{std::nullopt};

    Debug::CEntityWidget& mEntityWidget;
    Scene::ISceneHandler& mSceneHandler;
};
} // namespace Debug
