#pragma once

#include <memory>

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
    void DrawNodeRecursive(const Core::CGameObject& obj);

    Debug::CEntityWidget& mEntityWidget;
    Scene::ISceneHandler& mSceneHandler;
};
} // namespace Debug