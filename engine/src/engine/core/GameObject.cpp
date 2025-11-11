#include "engine/core/GameObject.h"
#include "engine/component/ComponentManager.h"
#include "engine/component/TransformComponent.h"

namespace Core {
CGameObject::CGameObject(const std::string& name,
                         Component::CComponentManager& componentManager,
                         CGameObject* parent, GameObjectId id)
    : mName(name)
    , mComponentManager(componentManager)
    , mParent(parent)
    , mId(id)
    , mTransformComponent(componentManager.addTransformComponent(*this)) {
}

GameObjectId CGameObject::getId() const {
    return mId;
}

CGameObject::~CGameObject() {
    mComponentManager.removeComponents(mId);
    if (mParent) {
        mParent = nullptr;
    }

    for (auto& child : mChildren) {
        child->mParent = nullptr;
    }

    mChildren.clear();
}

void CGameObject::destroySelf() {
    if (mParent) {
        mParent->removeChild(this);
        mParent = nullptr;
    } else {
        // Note: If no parent, root object, will be destroyed by scene
    }
}

CGameObject& CGameObject::addChild(std::unique_ptr<CGameObject>&& child) {
    mChildren.emplace_back(std::move(child));
    return *mChildren.back();
}

void CGameObject::removeChild(GameObjectId id) {
    auto it = std::find_if(mChildren.begin(), mChildren.end(),
                           [id](auto& obj) { return obj->getId() == id; });
    if (it != mChildren.end()) {
        mChildren.erase(it);
    }
}

void CGameObject::removeChildren() {
    mChildren.clear();
}

void CGameObject::removeChild(CGameObject* child) {
    auto it = std::find_if(mChildren.begin(), mChildren.end(),
                           [child](auto& obj) { return obj.get() == child; });
    if (it != mChildren.end()) {
        mChildren.erase(it);
    }
}

void CGameObject::setParent(CGameObject* parent) {
    if (!mParent) {
        return;
    }
    auto ptrToSelf = mParent->extractChild(this);
    if (ptrToSelf) {
        mParent = parent;
        mParent->addChild(std::move(ptrToSelf));
    }
}

std::unique_ptr<CGameObject> CGameObject::extractChild(CGameObject* child) {
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

glm::vec3 CGameObject::getLocalPosition() const {
    return mTransformComponent.getPosition();
}

glm::vec3 CGameObject::getWorldPosition() const {
    if (mParent) {
        return mParent->getWorldPosition() + getLocalPosition();
    }
    return getLocalPosition();
}

const std::vector<std::unique_ptr<CGameObject>>&
CGameObject::getChildren() const {
    return mChildren;
}

const std::string& CGameObject::getName() const {
    return mName;
}

bool CGameObject::IsVisible() const {
    return mIsVisible;
}

void CGameObject::SetVisible(bool isVisible) {
    mIsVisible = isVisible;
}

} // namespace Core
