#pragma once
#include <deque>
#include <vector>

#include "engine/core/GameObject.h"
#include "engine/core/GameObjectId.h"
#include "engine/renderer/TransformManager.h"

namespace Core {
class CGameObjectManager {
public:
    explicit CGameObjectManager();

    GameObjectId CreateObject(const std::string& name);
    void Destroy(GameObjectId id);

    Renderer::CTransformManager& GetTransformManager();

    CStringID GetName(GameObjectId id);
    void SetName(GameObjectId id, CStringID name);

    GameObjectId GetId(int index) const;

    void Update();

private:
    struct SGameObjectSlot {
        SGameObject mObject;
        uint32_t mGeneration{0};
        bool mExists{false};
    };
    std::vector<SGameObjectSlot> mGameObjects;
    std::deque<GameObjectId>
        mNextAvailableObject; // If empty, just push back in mGameObjects.

    Renderer::CTransformManager mTransformManager;
};
} // namespace Core
