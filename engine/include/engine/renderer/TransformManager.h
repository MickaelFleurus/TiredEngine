#pragma once

#include <set>
#include <unordered_set>
#include <vector>

#include "engine/core/GameObjectId.h"
#include "engine/renderer/DirtyObserver.h"
#include "engine/renderer/TransformData.h"
#include "engine/renderer/TransformHandle.h"
#include "engine/utils/GuardedContainer.h"
namespace Core {
class CGameObjectManager;
}

namespace Renderer {
class CTransformManager {
public:
    explicit CTransformManager(Core::CGameObjectManager& gameObjectManager);

    void Reset(uint32_t id);
    const glm::mat4& GetLocal(Core::GameObjectId id) const;
    const glm::mat4& GetWorld(Core::GameObjectId id) const;
    std::optional<Core::GameObjectId> GetParent(Core::GameObjectId id) const;
    const std::vector<Core::GameObjectId>&
    GetChildren(Core::GameObjectId id) const;

    void SetLocal(Core::GameObjectId id, const glm::mat4& matrix);
    void SetWorld(Core::GameObjectId id, const glm::mat4& matrix);
    void SetParent(Core::GameObjectId id, Core::GameObjectId parent);

    void AddChild(Core::GameObjectId id, Core::GameObjectId child);
    void RemoveChild(Core::GameObjectId id, Core::GameObjectId child);

    void SetPosition(Core::GameObjectId id, const glm::vec3& pos);
    void SetScale(Core::GameObjectId id, const glm::vec3& scale);
    void SetRotation(Core::GameObjectId id, const glm::quat& rot);

    const glm::vec3& GetPosition(Core::GameObjectId id) const;
    const glm::vec3& GetScale(Core::GameObjectId id) const;
    const glm::quat& GetRotation(Core::GameObjectId id) const;

    void RegisterObserver(IDirtyObserver& observer, CToken& token);

    bool HasRoots() const;
    const std::vector<Core::GameObjectId>& GetRoots() const;

    CTransformHandle CreateHandle(Core::GameObjectId id);
    void Update();

    void Clone(Core::GameObjectId dest, Core::GameObjectId src);

private:
    void AddDirtyTransform(Core::GameObjectId id);
    void RebuildLevels();
    void UpdateDirties();
    void NotifyAll(Core::GameObjectId id);

    Core::CGameObjectManager& mGameObjectManager;

    std::vector<Renderer::STransformBase> mBases;
    std::vector<Renderer::STransformExtended> mExtended;
    std::vector<Renderer::STransformExtra> mExtras;
    std::set<Core::GameObjectId> mDirtyLocal;
    std::unordered_set<Core::GameObjectId, Core::GameObjectIdHash>
        mDirtyTransform;

    std::vector<std::vector<Core::GameObjectId>> mLevels;
    bool mRebuildLevels{false};

    CGuardedContainer<IDirtyObserver> mObservers;
};
} // namespace Renderer
