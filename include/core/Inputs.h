#pragma once

namespace Core {
class CInputs {
public:
    CInputs();
    ~CInputs();

    bool Poll();
};
} // namespace Core