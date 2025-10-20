#pragma once

namespace Debug {
class COverlord;
}

namespace Core {
class CInputs {
public:
    CInputs(Debug::COverlord& overlord);
    ~CInputs();

    bool Poll();

private:
    Debug::COverlord& mOverlord;
};
} // namespace Core