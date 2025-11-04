#include "engine/utils/StringUtils.h"
#include <algorithm>
#include <cctype>
#include <ranges>

namespace {

bool ichar_equals(char a, char b) {
    return std::tolower(static_cast<unsigned char>(a)) ==
           std::tolower(static_cast<unsigned char>(b));
}
} // namespace

namespace Utils {
bool CompareIgnoreCase(std::string_view str1, std::string_view str2) {
    return std::ranges::equal(str1, str2, ichar_equals);
}
} // namespace Utils