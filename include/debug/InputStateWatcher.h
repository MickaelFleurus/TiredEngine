#pragma once

#include "debug/IOverlordItem.h"
#include "input/InputWatcher.h"
#include "utils/Token.h"

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