#pragma once

#include "engine/core/GameObject.h"
#include "engine/input/InputCallback.h"
// #include "engine/physics/CollisionInfo.h"
#include <functional>
#include <optional>
#include <string>

#include "engine/core/MeshTypes.h"
#include "engine/material/MaterialTypes.h"
#include "engine/utils/Anchors.h"

namespace Component {
class CComponentManager;
}

namespace Scene {
class CAbstractScene;
}

namespace Font {
class CFontHandler;
}

namespace Renderer {
class CMeshRenderer;
}

namespace Core {

class CMeshManager;

class CGameObjectBuilder {
public:
    struct CInputCallbacks {
        std::optional<Input::InputFunc> onSpacePressed{std::nullopt};
        std::optional<Input::InputFunc> onLeftPressed{std::nullopt};
        std::optional<Input::InputFunc> onRightPressed{std::nullopt};
    };

    class CBuilder {
    public:
        CBuilder(const std::string& name,
                 Component::CComponentManager& componentManager,
                 Font::CFontHandler& fontHandler, CMeshManager& meshFactory,
                 CGameObject& parent);

        CBuilder& AddText(const std::string& text, unsigned int size,
                          std::string fontName = "Arial");
        CBuilder& AddSprite();
        CBuilder& AddAABBCollisionData(float width, float height,
                                       bool isStatic = true,
                                       bool isTrigger = false);
        CBuilder& AddCircleCollisionData(float radius, bool isStatic = true,
                                         bool isTrigger = false);
        CBuilder& AddCameraComponent();
        // CBuilder& addCollisionCallback(
        //     std::function<void(Physics::SCollisionInfo)> callback);
        CBuilder& AddMovementData(float acceleration);
        CBuilder& AddInputInfo(CInputCallbacks callbacks);
        CBuilder& SetLocalPosition(const glm::vec3& position);
        CBuilder& SetAnchor(Utils::EAnchors anchor);
        CBuilder& Add3DCube(float size);
        CBuilder& Add3DQuad(float width, float height, float depth);
        CBuilder& SetMaterialType(Material::EMaterialType type);
        CGameObject* Build();

    private:
        Component::CComponentManager& mComponentManager;
        Font::CFontHandler& mFontHandler;
        CMeshManager& mMeshManager;
        CGameObject& mParent;
        std::unique_ptr<CGameObject> mGameObject;
    };

    explicit CGameObjectBuilder(Component::CComponentManager& componentManager,
                                Font::CFontHandler& fontHandler,
                                CMeshManager& meshFactory);
    CBuilder CreateBuilder(const std::string& name, CGameObject& parent);

private:
    static CGameObject*
    CreateRoot(Component::CComponentManager& componentManager);

    static GameObjectId mNextId;
    Component::CComponentManager& mComponentManager;
    Font::CFontHandler& mFontHandler;
    CMeshManager& mMeshManager;

    friend class Scene::CAbstractScene;
};

} // namespace Core