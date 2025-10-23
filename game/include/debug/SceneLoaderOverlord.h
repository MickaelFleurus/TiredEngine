#pragma once
#include <engine/debug/IOverlordItem.h>

namespace Scene {
class CSceneHandler;
}

namespace Debug {
class CSceneLoaderOverlord : public IOverlordItem {
public:
    CSceneLoaderOverlord(Scene::CSceneHandler& sceneLoader);
    void Render() override;
    const char* GetName() const override;

private:
    Scene::CSceneHandler& mSceneLoader;
};
} // namespace Debug