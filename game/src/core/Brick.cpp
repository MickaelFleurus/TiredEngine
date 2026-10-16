#include "core/Brick.h"

namespace Core {
void CBrick::Initialize(CGameObject* entity, int hitPoints, EBrickType type,
                        EBrickColor color) {
    mEntity = entity;
    mHitPoints = hitPoints;
    mType = type;
    mColor = color;
}

bool CBrick::IsDestroyed() const {
    return mHitCount >= mHitPoints;
}

} // namespace Core