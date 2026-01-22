#pragma once

#include "engine/core/GameObjectId.h"
#include "engine/utils/StringId.h"

namespace Core {
struct SGameObject {
    CStringID mName{""};
    GameObjectId mId;
    bool mIsActive;
};
} // namespace Core
