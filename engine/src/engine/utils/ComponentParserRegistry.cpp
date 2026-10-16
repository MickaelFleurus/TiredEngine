#include "engine/utils/ComponentParserRegistry.h"

void ComponentRegistry::Register(const std::string& componentName,
                                 ComponentDeserializer fn) {
    Get().InternalRegister(componentName, std::move(fn));
}

bool ComponentRegistry::Apply(const std::string& componentName,
                              Component::CManager& manager,
                              Core::SEntity entity,
                              const nlohmann::json& data) {
    return Get().InternalApply(componentName, manager, entity, data);
}

void ComponentRegistry::InternalRegister(const std::string& componentName,
                                         ComponentDeserializer fn) {
    mDeserializers[componentName] = std::move(fn);
}

bool ComponentRegistry::InternalApply(const std::string& componentName,
                                      Component::CManager& manager,
                                      Core::SEntity entity,
                                      const nlohmann::json& data) const {
    auto it = mDeserializers.find(componentName);
    if (it == mDeserializers.end())
        return false;
    it->second(manager, entity, data);
    return true;
}
