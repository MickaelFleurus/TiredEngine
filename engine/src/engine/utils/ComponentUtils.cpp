#include "engine/utils/ComponentUtils.h"

namespace Utils {

std::optional<bool> ParseBool(const nlohmann::json& j, std::string name) {
    if (j.contains(name)) {
        const auto& a = j.at(name);
        if (a.is_boolean()) {
            return a.get<bool>();
        }
    }
    return std::nullopt;
}

} // namespace Utils
