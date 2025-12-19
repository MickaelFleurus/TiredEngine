#include "core/Grid.h"

#include <algorithm>

#include <engine/core/GameObjectBuilder.h>

namespace Core {

CGrid::CGrid(CGameObjectBuilder& builder,
             Component::CComponentManager& componentManager,
             CGameObject& parent, int width, int height)
    : mBuilder(builder)
    , mComponentManager(componentManager)
    , mEntity(*builder.CreateBuilder("Grid", parent).Build())
    , mSize(width, height) {
    mBricks.resize(height);
    for (auto& row : mBricks) {
        row.resize(width);
    }
    mBricksPlaceholders.emplace_back();
}

void CGrid::AddBrickToRow(int hitPoints, EBrickType type, EBrickColor color) {
    mBricksPlaceholders.back().emplace_back(SBrickPlaceholder{
        .hitPoints = hitPoints, .type = type, .color = color});
}

void CGrid::NextRow() {
    if (mBricksPlaceholders.size() >= static_cast<size_t>(mSize.y)) {
        return;
    }
    mBricksPlaceholders.emplace_back();
}

bool CGrid::IsComplete() const {
    return !std::any_of(mBricks.cbegin(), mBricks.cend(), [](const auto& row) {
        return std::any_of(row.cbegin(), row.cend(), [](const auto& brick) {
            return brick.IsDestroyed() == false;
        });
    });
}

void CGrid::GenerateGrid(EGridHorizontalAlignment hAlign,
                         EGridVerticalAlignment vAlign) {
    int startRow = 0;
    if (vAlign == EGridVerticalAlignment::Center) {
        startRow = (mSize.y - mBricksPlaceholders.size()) / 2;
    } else if (vAlign == EGridVerticalAlignment::Bottom) {
        startRow = mSize.y - mBricksPlaceholders.size();
    }

    int startColumn = 0;
    int maxBricksInRow = 0;
    for (const auto& row : mBricksPlaceholders) {
        maxBricksInRow = std::max(maxBricksInRow, static_cast<int>(row.size()));
    }
    if (hAlign == EGridHorizontalAlignment::Center) {
        startColumn = (mSize.x - maxBricksInRow) / 2;
    } else if (hAlign == EGridHorizontalAlignment::Right) {
        startColumn = mSize.x - maxBricksInRow;
    }

    for (size_t r = 0; r < mBricksPlaceholders.size(); ++r) {
        const auto& rowPlaceholders = mBricksPlaceholders[r];
        for (size_t c = 0; c < rowPlaceholders.size(); ++c) {
            const auto& placeholder = rowPlaceholders[c];
            int gridRow = startRow + static_cast<int>(r);
            int gridCol = startColumn + static_cast<int>(c);
            if (gridRow >= 0 && gridRow < mSize.y && gridCol >= 0 &&
                gridCol < mSize.x) {
                CGameObject* brickEntity =
                    mBuilder.CreateBuilder("Brick", mEntity)
                        .AddSprite(placeholder.SpriteName())
                        .SetLocalPosition(
                            glm::vec3(gridCol * 64.0f, gridRow * 32.0f, 0.0f))
                        .Build();
                mBricks[gridRow][gridCol].Initialize(
                    brickEntity, placeholder.hitPoints, placeholder.type,
                    placeholder.color);
            }
        }
    }
}

} // namespace Core