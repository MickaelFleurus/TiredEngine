#pragma once

#include "engine/debug/IOverlordItem.h"

namespace Scene {
class CSceneHandler;
}

namespace Debug {
class CSceneHierarchy : public IOverlordItem {
public:
    CSceneHierarchy(Scene::CSceneHandler& sceneHandler);

    void Render() override;
    const char* GetName() const override;

private:
    Scene::CSceneHandler& mSceneHandler;
};
} // namespace Debug