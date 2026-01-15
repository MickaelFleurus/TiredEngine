#pragma once
#include <glm/vec3.hpp>

#include "engine/core/Camera.h"

namespace Core {
class CCameraUI : public CCamera {
public:
    explicit CCameraUI();

    void SetOrthographicSize(float size);
    float GetOrthographicSize() const;

protected:
    void EnsureUpToDate() override;

private:
    float mOrthographicSize{1080.0f};
};
} // namespace Core
