#pragma once

#include <vector>

namespace Renderer {

template <typename T>
class CRenderables {
public:
    CRenderables() = default;
    ~CRenderables() = default;

    void AddRenderable(const T& renderable, bool triggerReorder = false) {
        if (triggerReorder) {
            mReorderRenderables.push_back(renderable);
        } else {
            mUpdateRenderables.push_back(renderable);
        }
    }

    std::vector<T>& GetUpdateRenderables() {
        return mUpdateRenderables;
    }

    std::vector<T>& GetReorderRenderables() {
        return mReorderRenderables;
    }

    bool IsEmpty() const {
        return mUpdateRenderables.empty() && mReorderRenderables.empty();
    }

    void Clear() {
        mUpdateRenderables.clear();
        mReorderRenderables.clear();
    }

private:
    std::vector<T> mUpdateRenderables;
    std::vector<T> mReorderRenderables;
};
} // namespace Renderer