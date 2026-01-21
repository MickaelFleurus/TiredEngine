#include "engine/renderer/TransformManager.h"

#include "engine/core/GameObjectManager.h"
#include "engine/utils/Logger.h"

namespace {
constexpr int kDefaultVectorSize{1000};

}

namespace Renderer {
CTransformManager::CTransformManager(
    Core::CGameObjectManager& gameObjectManager)
    : mGameObjectManager(gameObjectManager) {
    mBases.reserve(kDefaultVectorSize);
    mExtended.reserve(kDefaultVectorSize);
    mExtras.reserve(kDefaultVectorSize);
}

void CTransformManager::Reset(uint32_t id) {
    if (id >= mBases.size()) {
        mBases.emplace_back();
        mExtended.emplace_back();
        mExtras.emplace_back();
    } else {
        mBases[id] = {};
        mExtended[id] = {};
        mExtras[id] = {};
    }
    mRebuildLevels = true;
}

const glm::mat4& CTransformManager::GetLocal(Core::GameObjectId id) const {
    return mBases[id.index].mLocal;
}

const glm::mat4& CTransformManager::GetWorld(Core::GameObjectId id) const {
    return mBases[id.index].mWorld;
}

std::optional<Core::GameObjectId>
CTransformManager::GetParent(Core::GameObjectId id) const {
    return mExtras[id.index].mParent;
}

const std::vector<Core::GameObjectId>&
CTransformManager::GetChildren(Core::GameObjectId id) const {
    return mExtras[id.index].mChildren;
}

void CTransformManager::SetLocal(Core::GameObjectId id,
                                 const glm::mat4& matrix) {
    mBases[id.index].mLocal = matrix;
    AddDirtyTransform(id);
}

void CTransformManager::SetWorld(Core::GameObjectId id,
                                 const glm::mat4& matrix) {
    mBases[id.index].mWorld = matrix;
    AddDirtyTransform(id);
}

void CTransformManager::SetParent(Core::GameObjectId id,
                                  Core::GameObjectId parent) {
    mExtras[id.index].mParent = parent;
    AddDirtyTransform(id);
    mRebuildLevels = true;
}

void CTransformManager::AddChild(Core::GameObjectId id,
                                 Core::GameObjectId child) {
    mExtras[id.index].mChildren.push_back(child);
    mRebuildLevels = true;
}

void CTransformManager::RemoveChild(Core::GameObjectId id,
                                    Core::GameObjectId child) {
    auto& children = mExtras[id.index].mChildren;
    children.erase(std::remove(children.begin(), children.end(), child),
                   children.end());
}

void CTransformManager::SetPosition(Core::GameObjectId id,
                                    const glm::vec3& pos) {
    mExtended[id.index].mPosition = pos;
    mDirtyLocal.insert(id);
    AddDirtyTransform(id);
}

void CTransformManager::SetScale(Core::GameObjectId id,
                                 const glm::vec3& scale) {
    mExtended[id.index].mScale = scale;
    mDirtyLocal.insert(id);
    AddDirtyTransform(id);
}

void CTransformManager::SetRotation(Core::GameObjectId id,
                                    const glm::quat& rot) {
    mExtended[id.index].mRotation = rot;
    mDirtyLocal.insert(id);
    AddDirtyTransform(id);
}

const glm::vec3& CTransformManager::GetPosition(Core::GameObjectId id) const {
    return mExtended[id.index].mPosition;
}

const glm::vec3& CTransformManager::GetScale(Core::GameObjectId id) const {
    return mExtended[id.index].mScale;
}

const glm::quat& CTransformManager::GetRotation(Core::GameObjectId id) const {
    return mExtended[id.index].mRotation;
}

void CTransformManager::RegisterObserver(IDirtyObserver& observer,
                                         CToken& token) {
    mObservers.Add(observer, token);
}

bool CTransformManager::HasRoots() const {
    return !mLevels.empty();
}

const std::vector<Core::GameObjectId>& CTransformManager::GetRoots() const {
    if (mLevels.empty()) {
        LOG_FATAL("Trying to get the roots when there is none.");
    }
    return mLevels[0];
}

CTransformHandle CTransformManager::CreateHandle(Core::GameObjectId id) {
    return CTransformHandle(*this, id);
}
void CTransformManager::Update() {
    RebuildLevels();
    UpdateDirties();
}

void CTransformManager::AddDirtyTransform(Core::GameObjectId id) {
    mDirtyTransform.insert(id);
    NotifyAll(id);
    for (auto childId : mExtras[id.index].mChildren) {
        AddDirtyTransform(childId);
    }
}

// This should only be called before resolving the transforms, not every time a
// parent changes
void CTransformManager::RebuildLevels() {
    if (!mRebuildLevels) {
        return;
    }
    mLevels.clear();
    mLevels.push_back({});
    int i = 0;
    for (const auto& transform : mExtras) {
        if (!transform.mParent.has_value()) {
            mLevels[0].push_back(mGameObjectManager.GetId(i));
        }
        i++;
    }

    i = 0;
    while (!mLevels[i].empty()) {
        std::vector<Core::GameObjectId> children;
        for (auto id : mLevels[i]) {
            if (!mExtras[id.index].mChildren.empty()) {
                std::copy(mExtras[id.index].mChildren.begin(),
                          mExtras[id.index].mChildren.end(),
                          std::back_inserter(children));
            }
        }
        mLevels.push_back(children);
        i++;
    }
    mRebuildLevels = false;
}

void CTransformManager::UpdateDirties() {

    if (!mDirtyLocal.empty()) {
        for (auto id : mDirtyLocal) {
            auto& localTransforms = mExtended[id.index];
            mBases[id.index].mLocal =
                glm::translate(glm::mat4{1.0f}, localTransforms.mPosition) *
                glm::mat4_cast(localTransforms.mRotation) *
                glm::scale(glm::mat4{1.0f}, localTransforms.mScale);
        }
    }

    if (mDirtyTransform.empty()) {
        return;
    }

    for (const auto& level : mLevels) {
        for (auto id : level) {
            if (mDirtyTransform.count(id)) {
                glm::mat4 parentWorld{1.0f};
                if (mExtras[id.index].mParent.has_value()) {
                    parentWorld =
                        mBases[mExtras[id.index].mParent.value().index].mWorld;
                }
                mBases[id.index].mWorld = mBases[id.index].mLocal * parentWorld;
            }
        }
    }
    mDirtyTransform.clear();
}

void CTransformManager::NotifyAll(Core::GameObjectId id) {
    for (auto& observer : mObservers) {
        observer.OnDirty(id);
    }
}
} // namespace Renderer
