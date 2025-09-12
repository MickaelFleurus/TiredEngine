#pragma once

namespace Debug {
class IOverlordItem {
public:
    virtual ~IOverlordItem() = default;
    virtual void Render() = 0;
};
} // namespace Debug