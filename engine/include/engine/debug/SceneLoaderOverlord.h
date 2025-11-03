#pragma once
#include "engine/debug/IOverlordItem.h"

namespace Scene {
class ISceneHandler;
}

namespace Debug {
class CSceneLoaderOverlord : public IOverlordItem {
public:
    CSceneLoaderOverlord(Scene::ISceneHandler& sceneHandler);
    void Render() override;
    const char* GetName() const override;

private:
    Scene::ISceneHandler& mSceneHandler;
};
} // namespace Debug