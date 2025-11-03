#pragma once
#include "engine/scene/AbstractSceneHandler.h"

namespace Scene {

class CSceneHandler : public CAbstractSceneHandler {
public:
    explicit CSceneHandler(Core::CEngineLoop& engineLoop,
                           Component::CComponentManager& componentManager,
                           Core::CWindowData& windowData);

    void CreateAndSetScene(const std::string& sceneName) override;
    const std::vector<std::string>& GetSceneNames() const override;
};
} // namespace Scene