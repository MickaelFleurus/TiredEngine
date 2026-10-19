#pragma once

#include <optional>
#include <string>

#include <glm/detail/type_vec2.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <nlohmann/json.hpp>

#include "nlohmann/json_fwd.hpp"
namespace Utils {

std::optional<bool> ParseBool(const nlohmann::json& j, std::string name);

template <typename T>
std::optional<glm::vec<2, T, glm::defaultp>>
ParseArray2(const nlohmann::json& j, std::string name) {
    if (j.contains(name)) {
        const auto& a = j.at(name);
        if (a.is_array() && a.size() == 2) {
            return glm::vec2{a.at(0).get<T>(), a.at(1).get<T>()};
        }
    }
    return std::nullopt;
}

template <typename T>
std::optional<glm::vec<3, T, glm::defaultp>>
ParseArray3(const nlohmann::json& j, std::string name) {
    if (j.contains(name)) {
        const auto& a = j.at(name);
        if (a.is_array() && a.size() == 3) {
            return glm::vec3{a.at(0).get<T>(), a.at(1).get<T>(),
                             a.at(2).get<T>()};
        }
    }
    return std::nullopt;
}

template <typename T>
std::optional<glm::vec<4, T, glm::defaultp>>
ParseArray4(const nlohmann::json& j, std::string name) {
    if (j.contains(name)) {
        const auto& a = j.at(name);
        if (a.is_array() && a.size() == 4) {
            return glm::vec4{a.at(0).get<T>(), a.at(1).get<T>(),
                             a.at(2).get<T>(), a.at(3).get<T>()};
        }
    }
    return std::nullopt;
}
} // namespace Utils
