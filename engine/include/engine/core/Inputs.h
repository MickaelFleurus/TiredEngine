#pragma once

namespace Debug {
class COverlordManager;
}

namespace Core {
class CInputs {
public:
    CInputs(Debug::COverlordManager& overlord);
    ~CInputs();

    bool Poll();

private:
    Debug::COverlordManager& mOverlordManager;
};
} // namespace Core