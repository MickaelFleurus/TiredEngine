#pragma once

#include "engine/debug/IOverlordItem.h"
#include "engine/input/InputWatcher.h"
#include "engine/utils/Token.h"

namespace Debug {
class CInputStateWatcher : public IOverlordItem {
public:
    CInputStateWatcher();
    void Render() override;
    const char* GetName() const override;

private:
    Input::CInputWatcher mInputStateWatcher;
    CToken mToken;
};
} // namespace Debug