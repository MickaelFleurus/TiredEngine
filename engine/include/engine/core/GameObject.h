#pragma once

#include "engine/core/GameObjectId.h"
#include <glm/vec3.hpp>

#include <memory>
#include <optional>
#include <vector>

namespace Component {
class CComponentManager;
class CTransformComponent;
} // namespace Component

namespace Core {
class CGameObject {
public:
    ~CGameObject();

    glm::vec3 getLocalPosition() const;
    glm::vec3 getWorldPosition() const;

    void removeChild(GameObjectId id);
    void removeChildren();
    void destroySelf();

    const std::string& getName() const;

    GameObjectId getId() const;
    void setParent(CGameObject* parent);

    bool IsVisible() const;
    void SetVisible(bool isVisible);

    const std::vector<std::unique_ptr<CGameObject>>& getChildren() const;

private:
    CGameObject(const std::string& name,
                Component::CComponentManager& componentManager,
                CGameObject* mParent = nullptr, GameObjectId id = 0);
    void free();

    std::unique_ptr<CGameObject> extractChild(CGameObject* child);

    CGameObject& addChild(std::unique_ptr<CGameObject>&& child);
    void removeChild(CGameObject* child);

    const std::string mName;
    bool mIsVisible{true};

    Component::CComponentManager& mComponentManager;
    CGameObject* mParent{nullptr};
    std::vector<std::unique_ptr<CGameObject>> mChildren;
    const GameObjectId mId;
    Component::CTransformComponent& mTransformComponent;

    friend class CGameObjectBuilder;
};
} // namespace Core