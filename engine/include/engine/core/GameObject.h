#pragma once

#include "engine/core/GameObjectId.h"
#include <glm/vec2.hpp>

#include <memory>
#include <optional>
#include <vector>

namespace Component {
class CComponentManager;
}

namespace Core {
class CGameObject {
public:
    ~CGameObject();

    void setLocalPosition(const glm::vec2& position);

    glm::vec2 getLocalPosition() const;
    glm::vec2 getWorldPosition() const;

    void removeChild(GameObjectId id);
    void removeChildren();
    void destroySelf();

    GameObjectId getId() const;
    void setParent(CGameObject* parent);

    const std::vector<std::unique_ptr<CGameObject>>& getChildren() const;

private:
    CGameObject(Component::CComponentManager& componentManager,
                CGameObject* mParent = nullptr, GameObjectId id = 0);
    void free();

    std::unique_ptr<CGameObject> extractChild(CGameObject* child);

    CGameObject& addChild(std::unique_ptr<CGameObject>&& child);
    void removeChild(CGameObject* child);

    glm::vec2 mLocalPosition{};
    glm::vec2 mAnchorPosition{0.0f, 0.0f};
    Component::CComponentManager& mComponentManager;
    CGameObject* mParent{nullptr};
    std::vector<std::unique_ptr<CGameObject>> mChildren;
    const GameObjectId mId;

    friend class CGameObjectBuilder;
};
} // namespace Core