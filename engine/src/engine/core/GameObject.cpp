#include "engine/core/GameObject.h"

#include "engine/component/CameraComponent.h"
#include "engine/component/ComponentManager.h"
#include "engine/component/TransformComponent.h"
#include "engine/renderer/RendererManager.h"

namespace {

static Core::GameObjectId mNextId = 0;
}

namespace Core {

CGameObject::CGameObject(const std::string& name,
                         Component::CComponentManager& componentManager,
                         CGameObject* parent)
    : mName(name)
    , mComponentManager(componentManager)
    , mParent(parent)
    , mId(mNextId++)
    , mTransformComponent(componentManager.AddTransformComponent(*this)) {
}

GameObjectId CGameObject::GetId() const {
    return mId;
}

CGameObject::~CGameObject() {
    mComponentManager.RemoveComponents(mId);
    if (mParent) {
        mParent = nullptr;
    }

    for (auto& child : mChildren) {
        child->mParent = nullptr;
    }

    mChildren.clear();
    Renderer::CRendererManager::NotifyGameObjectHidden(mId);
}

void CGameObject::DestroySelf() {
    if (mParent) {
        mParent->RemoveChild(this);
    } else {
        // Note: If no parent, root object, will be destroyed by scene
    }
}

CGameObject* CGameObject::Clone(CGameObject* parent) const {
    std::unique_ptr<CGameObject> clone = std::unique_ptr<CGameObject>(
        new CGameObject(mName + "_copy", mComponentManager));

    CGameObject* rawPtr = clone.get();
    clone->mIsVisible = this->mIsVisible;
    parent->AddChild(std::move(clone));
    mComponentManager.CloneComponents(*rawPtr, *this);

    return rawPtr;
}

CGameObject& CGameObject::AddChild(std::unique_ptr<CGameObject>&& child) {
    child->SetParent(this);
    mChildren.emplace_back(std::move(child));
    return *mChildren.back();
}

void CGameObject::RemoveChild(GameObjectId id) {
    auto it = std::find_if(mChildren.begin(), mChildren.end(),
                           [id](auto& obj) { return obj->GetId() == id; });
    if (it != mChildren.end()) {
        mChildren.erase(it);
    }
}

void CGameObject::RemoveChildren() {
    mChildren.clear();
}

void CGameObject::RemoveChild(CGameObject* child) {
    auto it = std::find_if(mChildren.begin(), mChildren.end(),
                           [child](auto& obj) { return obj.get() == child; });
    if (it != mChildren.end()) {
        mChildren.erase(it);
    }
}

void CGameObject::SetParent(CGameObject* parent) {
    mParent = parent;
}

CGameObject* CGameObject::GetParent() const {
    return mParent;
}

std::unique_ptr<CGameObject> CGameObject::ExtractChild(CGameObject* child) {
    auto it = std::find_if(mChildren.begin(), mChildren.end(),
                           [child](auto& obj) { return obj.get() == child; });
    if (it != mChildren.end()) {
        auto extracted = std::move(*it);
        mChildren.erase(it);
        return extracted;
    }
    std::unreachable();
    return nullptr;
}

glm::vec3 CGameObject::GetLocalPosition() const {
    return mTransformComponent.GetPosition();
}

glm::vec3 CGameObject::GetWorldPosition() const {
    if (mParent) {
        return mParent->GetWorldPosition() + GetLocalPosition();
    }
    return GetLocalPosition();
}

glm::mat4 CGameObject::GetModelMatrix() const {
    if (mParent) {
        glm::mat4 parentModel = mParent->GetModelMatrix();
        mTransformComponent.UpdateMatrix(parentModel);
        return parentModel;
    }
    return glm::mat4(1.0f);
}

const std::vector<std::unique_ptr<CGameObject>>&
CGameObject::GetChildren() const {
    return mChildren;
}

const std::string& CGameObject::GetName() const {
    return mName;
}

void CGameObject::SetName(const std::string& name) {
    mName = name;
}

bool CGameObject::IsVisible() const {
    return mIsVisible;
}

void CGameObject::SetVisible(bool isVisible) {
    mIsVisible = isVisible;
    if (!isVisible) {
        Renderer::CRendererManager::NotifyGameObjectHidden(mId);
    }
}

void CGameObject::AddDirtyFlag(EDirtyType flag) {
    mDirtyFlags = mDirtyFlags | flag;
}

EDirtyType CGameObject::GetDirtyFlags() const {
    return mDirtyFlags;
}

void CGameObject::ClearDirtyFlags() {
    mDirtyFlags = EDirtyType::None;
}

void CGameObject::ReflectIfVisible() {
    constexpr std::array<Component::EComponentType, 2> visualComponent = {
        Component::EComponentType::Mesh, Component::EComponentType::TextUI};
    for (auto type : visualComponent) {
        if (mComponentManager.HasComponent(mId, type)) {
            SetVisible(true);
            return;
        }
    }
    SetVisible(false);
}

} // namespace Core
