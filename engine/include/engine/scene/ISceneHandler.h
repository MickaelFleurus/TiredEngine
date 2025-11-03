#pragma once
#include <string>
#include <vector>

namespace Scene {

class CAbstractScene;

class ISceneHandler {
public:
    virtual ~ISceneHandler() = default;

    virtual CAbstractScene* GetCurrentScene() const = 0;
    virtual const std::vector<std::string>& GetSceneNames() const = 0;
    virtual void CreateAndSetScene(const std::string& sceneName) = 0;
};
} // namespace Scene