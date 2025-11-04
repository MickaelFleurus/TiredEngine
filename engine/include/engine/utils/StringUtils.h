#pragma once

#include <string_view>

namespace Utils {
bool CompareIgnoreCase(std::string_view str1, std::string_view str2);
}