#pragma once

#include "engine/core/GameObject.h"
#include "engine/input/InputCallback.h"
// #include "engine/physics/CollisionInfo.h"

#include <functional>
#include <optional>
#include <string>

namespace Component {
class CComponentManager;
}

namespace Scene {
class CAbstractScene;
}

namespace Core {
class CGameObjectBuilder {
public:
    struct CInputCallbacks {
        std::optional<Input::InputFunc> onSpacePressed{std::nullopt};
        std::optional<Input::InputFunc> onLeftPressed{std::nullopt};
        std::optional<Input::InputFunc> onRightPressed{std::nullopt};
    };

    class CBuilder {
    public:
        CBuilder(const std::string& name,
                 Component::CComponentManager& componentManager,
                 CGameObject& parent);

        CBuilder& addText(const std::string& text, unsigned size);
        CBuilder& addSprite();
        CBuilder& addAABBCollisionData(float width, float height,
                                       bool isStatic = true,
                                       bool isTrigger = false);
        CBuilder& addCircleCollisionData(float radius, bool isStatic = true,
                                         bool isTrigger = false);
        // CBuilder& addCollisionCallback(
        //     std::function<void(Physics::SCollisionInfo)> callback);
        CBuilder& addMovementData(float acceleration);
        CBuilder& addInputInfo(CInputCallbacks callbacks);
        CBuilder& setLocalPosition(const glm::vec3& position);
        CGameObject* build();

    private:
        Component::CComponentManager& mComponentManager;
        CGameObject& mParent;
        std::unique_ptr<CGameObject> mGameObject;
    };

    explicit CGameObjectBuilder(Component::CComponentManager& componentManager);
    CBuilder CreateBuilder(const std::string& name, CGameObject& parent);

private:
    static CGameObject*
    createRoot(Component::CComponentManager& componentManager);

    static GameObjectId mNextId;
    Component::CComponentManager& mComponentManager;

    friend class Scene::CAbstractScene;
};

} // namespace Core