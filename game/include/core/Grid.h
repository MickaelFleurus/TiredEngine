#pragma once
#include <format>
#include <string>
#include <vector>

#include <glm/vec2.hpp>
#include <magic_enum/magic_enum.hpp>

#include "core/Brick.h"
#include "core/BrickType.h"

namespace Component {
class CComponentManager;
} // namespace Component

namespace Core {
class CGameObjectBuilder;
class CGameObject;

class CGrid {
public:
    enum class EGridHorizontalAlignment { Left, Center, Right };
    enum class EGridVerticalAlignment { Top, Center, Bottom };

    explicit CGrid(CGameObjectBuilder& builder,
                   Component::CComponentManager& componentManager,
                   CGameObject& parent, int width, int height);

    void AddBrickToRow(int hitPoints, EBrickType type, EBrickColor color);
    void NextRow();

    void GenerateGrid(EGridHorizontalAlignment hAlign,
                      EGridVerticalAlignment vAlign);

    bool IsComplete() const;

private:
    struct SBrickPlaceholder {
        int hitPoints = 0;
        EBrickType type = EBrickType::Standard;
        EBrickColor color = EBrickColor::Red;
        std::string SpriteName() const {
            return std::format("{}{}", magic_enum::enum_name(type),
                               magic_enum::enum_name(color));
        }
    };

    CGameObjectBuilder& mBuilder;
    Component::CComponentManager& mComponentManager;
    CGameObject& mEntity;
    glm::ivec2 mSize;
    std::vector<std::vector<CBrick>> mBricks;
    std::vector<std::vector<SBrickPlaceholder>> mBricksPlaceholders;
};
} // namespace Core