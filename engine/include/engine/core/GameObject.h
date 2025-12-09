#pragma once

#include "engine/core/DirtyTypeEnum.h"
#include "engine/core/GameObjectId.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace Component {
class CComponentManager;
class CTransformComponent;
} // namespace Component

namespace Core {
class CGameObject {
public:
    ~CGameObject();

    glm::vec3 GetLocalPosition() const;
    glm::vec3 GetWorldPosition() const;
    glm::mat4 GetModelMatrix() const;

    void RemoveChild(GameObjectId id);
    void RemoveChildren();
    void DestroySelf();

    const std::string& GetName() const;

    GameObjectId GetId() const;
    void SetParent(CGameObject* parent);

    bool IsVisible() const;
    void SetVisible(bool isVisible);

    const std::vector<std::unique_ptr<CGameObject>>& GetChildren() const;

    void AddDirtyFlag(EDirtyType flag);
    EDirtyType GetDirtyFlags() const;
    void ClearDirtyFlags();

private:
    CGameObject(const std::string& name,
                Component::CComponentManager& componentManager,
                CGameObject* parent = nullptr, GameObjectId id = 0);

    std::unique_ptr<CGameObject> ExtractChild(CGameObject* child);

    CGameObject& AddChild(std::unique_ptr<CGameObject>&& child);
    void RemoveChild(CGameObject* child);

    const std::string mName;
    bool mIsVisible{true};

    Component::CComponentManager& mComponentManager;
    CGameObject* mParent{nullptr};
    std::vector<std::unique_ptr<CGameObject>> mChildren;
    const GameObjectId mId;
    Component::CTransformComponent& mTransformComponent;
    EDirtyType mDirtyFlags{EDirtyType::New};

    friend class CGameObjectBuilder;
};
} // namespace Core