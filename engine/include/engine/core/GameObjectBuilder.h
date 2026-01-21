#pragma once

#include <functional>
#include <optional>
#include <string>

#include <glm/vec3.hpp>

#include "engine/core/GameObjectId.h"
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

namespace Core {

class CMeshManager;
class CGameObjectManager;

class CGameObjectBuilder {
public:
    explicit CGameObjectBuilder(Component::CComponentManager& componentManager,
                                Font::CFontHandler& fontHandler,
                                CMeshManager& meshFactory,
                                CGameObjectManager& gameObjectManager);

    CGameObjectBuilder& Start(const std::string& name);
    CGameObjectBuilder& AddText(const std::string& text, unsigned int size,
                                std::string fontName = "Arial");
    CGameObjectBuilder& AddSprite(std::string spriteName);
    CGameObjectBuilder& AddCameraComponent();
    CGameObjectBuilder& SetLocalPosition(const glm::vec3& position);
    CGameObjectBuilder& SetAnchor(Utils::EAnchors anchor);
    CGameObjectBuilder& Add3DCube(float size);
    CGameObjectBuilder& Add3DQuad(float width, float height, float depth);
    CGameObjectBuilder& SetMaterialType(Material::EMaterialType type);
    GameObjectId Build();

private:
    Component::CComponentManager& mComponentManager;
    Font::CFontHandler& mFontHandler;
    CMeshManager& mMeshManager;
    CGameObjectManager& mGameObjectManager;
    std::optional<GameObjectId> mCreatedObj{std::nullopt};
};

} // namespace Core
