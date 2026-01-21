#include "engine/core/GameObjectManager.h"

#include "engine/renderer/TransformManager.h"

namespace {
constexpr int kDefaultVectorSize{1000};
}

namespace Core {
CGameObjectManager::CGameObjectManager() : mTransformManager(*this) {
    mGameObjects.reserve(kDefaultVectorSize);
}

GameObjectId CGameObjectManager::CreateObject(const std::string& name) {
    GameObjectId id{};
    if (mNextAvailableObject.size() != 0) {
        id = mNextAvailableObject.front();
        id.generation++;
        mNextAvailableObject.pop_front();
    } else {
        id.index = mGameObjects.size();
        mGameObjects.push_back(SGameObjectSlot{});
    }

    SGameObjectSlot& slot = mGameObjects[id.index];
    slot.mExists = true;
    slot.mGeneration = id.generation;
    slot.mObject =
        SGameObject{.mId = id, .mIsActive = true, .mName = CStringID(name)};

    mTransformManager.Reset(id.index);
    return id;
}

void CGameObjectManager::Destroy(GameObjectId id) {
    auto it = std::find_if(mGameObjects.begin(), mGameObjects.end(),
                           [id](const SGameObjectSlot& slot) {
                               return slot.mGeneration == id.generation &&
                                      slot.mObject.mId == id;
                           });
    if (it != mGameObjects.end()) {
        it->mExists = false;
        mNextAvailableObject.push_back(id);
    }
}

Renderer::CTransformManager& CGameObjectManager::GetTransformManager() {
    return mTransformManager;
}

CStringID CGameObjectManager::GetName(GameObjectId id) {
    return mGameObjects.at(id.index).mObject.mName;
}

void CGameObjectManager::SetName(GameObjectId id, CStringID name) {
    mGameObjects[id.index].mObject.mName = name;
}

GameObjectId CGameObjectManager::GetId(int index) const {
    return mGameObjects.at(index).mObject.mId;
}

void CGameObjectManager::Update() {
    mTransformManager.Update();
}

} // namespace Core
