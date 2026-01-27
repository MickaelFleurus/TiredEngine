#pragma once
#include <string>
#include <vector>

#include <glm/mat4x4.hpp>

#include "engine/component/IComponent.h"
#include "engine/core/DataTypes.h"

namespace Font {
class CPolice;
}

namespace Component {
class CTextUIComponent : public IComponent {
public:
    explicit CTextUIComponent(Core::GameObjectId owner,
                              CComponentManager& componentManager);
    explicit CTextUIComponent(Core::GameObjectId owner,
                              CComponentManager& componentManager,
                              const CTextUIComponent& other);

    CTextUIComponent& operator=(const CTextUIComponent& other);
    CTextUIComponent(const CTextUIComponent& other) = delete;
    CTextUIComponent(const CTextUIComponent&& other) = delete;
    CTextUIComponent& operator=(CTextUIComponent&& other) = delete;
    ~CTextUIComponent() override;

    void SetText(const std::string& text);
    void SetPolice(Font::CPolice* police);

    Font::CPolice* GetPolice() const;
    int GetFontSize() const;
    void SetFontSize(int size);
    const glm::vec4& GetColor() const;
    void SetColor(const glm::vec4& color);
    const std::string& GetText() const;

    glm::vec2 GetSize() const;

    const std::vector<Core::SUIInstanceData>&
    GetInstances(const glm::mat4& entityModel = glm::mat4{1.0});

private:
    void GenerateInstances(const glm::mat4& entityModel);
    void ResolveSize();

    std::vector<Core::SUIInstanceData> mInstances;

    std::string mText;
    Font::CPolice* mPolice{nullptr};
    int mFontSize = 24;
    glm::vec2 mSize{0.0f, 0.0f};
    glm::vec4 mColor{1.0f, 1.0f, 1.0f, 1.0f};
};
} // namespace Component
