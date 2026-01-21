#include "engine/renderer/TransformHandle.h"

#include "engine/renderer/TransformManager.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

namespace Renderer {

CTransformHandle::CTransformHandle(CTransformManager& manager,
                                   Core::GameObjectId id)
    : mManager(manager), mId(id) {
}

void CTransformHandle::SetPosition(glm::vec3 position) {
    mManager.SetPosition(mId, position);
}

glm::vec3 CTransformHandle::GetPosition() const {
    return mManager.GetPosition(mId);
}

void CTransformHandle::Move(glm::vec3 move) {
    auto& position = mManager.GetPosition(mId);
    mManager.SetPosition(mId, position + move);
}

// FIXME
void CTransformHandle::SetRotation(glm::vec3 rotation) {
    rotation = glm::radians(rotation);
    mManager.SetRotation(
        mId, glm::quat(glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z)));
}

glm::vec3 CTransformHandle::GetRotation() const {
    auto quat = mManager.GetRotation(mId);
    glm::mat4 rotMat = glm::mat4_cast(quat);
    glm::vec3 euler = glm::eulerAngles(quat);
    return glm::degrees(euler);
}

void CTransformHandle::SetScale(const glm::vec3& scale) {
    mManager.SetScale(mId, scale);
}

glm::vec3 CTransformHandle::GetScale() const {
    return mManager.GetScale(mId);
}

bool CTransformHandle::HasChildren() const {
    return !mManager.GetChildren(mId).empty();
}

const std::vector<Core::GameObjectId>& CTransformHandle::GetChildren() const {
    return mManager.GetChildren(mId);
}
} // namespace Renderer
