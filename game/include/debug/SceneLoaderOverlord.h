#pragma once
#include <engine/debug/IOverlordItem.h>

namespace Scene {
class ISceneLoader;
}

namespace Debug {
class CSceneLoaderOverlord : public IOverlordItem {
public:
    CSceneLoaderOverlord(Scene::ISceneLoader& sceneLoader);
    void Render() override;
    const char* GetName() const override;

private:
    Scene::ISceneLoader& mSceneLoader;
};
} // namespace Debug