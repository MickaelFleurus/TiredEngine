#pragma once
#include <deque>
#include <vector>

#include "engine/core/GameObject.h"
#include "engine/core/GameObjectId.h"
#include "engine/renderer/TransformManager.h"

namespace Component {
class CComponentManager;
}

namespace Core {
class CGameObjectManager {
public:
    explicit CGameObjectManager();

    GameObjectId CreateObject(const std::string& name);
    GameObjectId CreateObject(CStringId name);
    void Destroy(GameObjectId id);

    Renderer::CTransformManager& GetTransformManager();

    CStringId GetStringId(GameObjectId id);
    void SetStringId(GameObjectId id, CStringId name);

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
