#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include "engine/core/Entity.h"
namespace Component {
class CManager;
}

using ComponentDeserializer = std::function<void(
    Component::CManager&, Core::SEntity, const nlohmann::json&)>;

class ComponentRegistry {
public:
    static void Register(const std::string& componentName,
                         ComponentDeserializer fn);

    static bool Apply(const std::string& componentName,
                      Component::CManager& manager, Core::SEntity entity,
                      const nlohmann::json& data);

protected:
    void InternalRegister(const std::string& componentName,
                          ComponentDeserializer fn);

    bool InternalApply(const std::string& componentName,
                       Component::CManager& manager, Core::SEntity entity,
                       const nlohmann::json& data) const;

private:
    static ComponentRegistry& Get() {
        static ComponentRegistry instance;
        return instance;
    }

    std::unordered_map<std::string, ComponentDeserializer> mDeserializers;
};

struct ComponentRegistrar {
    ComponentRegistrar(const std::string& name, ComponentDeserializer fn) {
        ComponentRegistry::Register(name, std::move(fn));
    }
};
