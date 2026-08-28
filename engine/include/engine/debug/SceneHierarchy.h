// #pragma once

// #include <memory>
// #include <optional>
// #include <string>

// #include "engine/core/GameObjectId.h"
// #include "engine/debug/IOverlordItem.h"

// namespace Scene {
// class ISceneHandler;
// }

// namespace Component {
// class CComponentManager;
// }

// namespace Core {
// class CGameObjectManager;
// }

// namespace Renderer {
// class CTransformManager;
// }

// namespace Debug {

// class CEntityWidget;

// class CSceneHierarchy : public IOverlordItem {
// public:
//     CSceneHierarchy(Component::CComponentManager& componentManager,
//                     Debug::CEntityWidget& entityWidget,
//                     Scene::ISceneHandler& sceneHandler,
//                     Core::CGameObjectManager& gameObjectManager);
//     ~CSceneHierarchy() override;

//     void Render() override;
//     const char* GetName() const override;

// private:
//     void DrawNodeRecursive(Core::GameObjectId obj);
//     void DrawContextMenu();
//     void DrawNameModal();

//     std::optional<Core::GameObjectId> mSelectedItem{std::nullopt};

//     Component::CComponentManager& mComponentManager;
//     Debug::CEntityWidget& mEntityWidget;
//     Scene::ISceneHandler& mSceneHandler;
//     Core::CGameObjectManager& mGameObjectManager;
//     Renderer::CTransformManager& mTransformManager;

//     std::optional<std::string> mModalName{std::nullopt};
// };
// } // namespace Debug
