#pragma once

namespace Debug {
class IOverlordItem {
public:
    virtual ~IOverlordItem() = default;

    virtual void Render() = 0;
    virtual const char* GetName() const = 0;
    bool IsVisible() const {
        return m_Visible;
    }
    void ToggleVisible() {
        m_Visible = !m_Visible;
    }

private:
    bool m_Visible = false;
};
} // namespace Debug