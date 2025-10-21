#pragma once

namespace Debug {
class IOverlordItem {
public:
    virtual ~IOverlordItem() = default;

    virtual void Render() = 0;
    virtual const char* GetName() const = 0;
    
    bool IsVisible() const {
        return mVisible;
    }

    void ToggleVisible() {
        mVisible = !mVisible;
    }

    void SetVisible(bool visible) {
        mVisible = visible;
    }

private:
    bool mVisible = false;
};
} // namespace Debug