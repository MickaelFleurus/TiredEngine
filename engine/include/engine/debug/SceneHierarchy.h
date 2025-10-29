#pragma once

#include <memory>

#include "engine/debug/IOverlordItem.h"

namespace Scene {
class CSceneHandler;
}

namespace Core {
class CGameObject;
}

namespace Debug {

class CEntityWidget;

class CSceneHierarchy : public IOverlordItem {
public:
    CSceneHierarchy(Debug::CEntityWidget& entityWidget,
                    Scene::CSceneHandler& sceneHandler);
    ~CSceneHierarchy() override;

    void Render() override;
    const char* GetName() const override;

private:
    void DrawNodeRecursive(const Core::CGameObject& obj);

    Debug::CEntityWidget& mEntityWidget;
    Scene::CSceneHandler& mSceneHandler;
};
} // namespace Debug