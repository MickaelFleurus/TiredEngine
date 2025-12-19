#pragma once
#include "core/BrickType.h"

namespace Core {
class CGameObject;
class CBrick {
public:
    CBrick() = default;
    void Initialize(CGameObject* entity, int hitPoints, EBrickType type,
                    EBrickColor color);

    bool IsDestroyed() const;

private:
    CGameObject* mEntity{nullptr};
    int mHitPoints{0};
    EBrickType mType{EBrickType::Standard};
    EBrickColor mColor{EBrickColor::Red};
    int mHitCount{0};
};
} // namespace Core