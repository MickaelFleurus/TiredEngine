#pragma once

#include "engine/debug/IOverlordItem.h"
namespace Debug {
class CCameraComponentWidget : public IOverlordItem {
public:
    CCameraComponentWidget();

    void Render() override;
    const char* GetName() const override;
};
} // namespace Debug