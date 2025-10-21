#pragma once
#include "scene/SceneEnum.h"

namespace Scene {
class ISceneLoader {
public:
    virtual ~ISceneLoader() = default;
    virtual void LoadScene(Scene::ESceneType sceneType) = 0;
};
}