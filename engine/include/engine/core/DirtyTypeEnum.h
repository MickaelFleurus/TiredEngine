#pragma once
#include <cstdint>

namespace Core {

enum class EDirtyType : uint8_t {
    None = 0,
    Transform = 1 << 0,
    InstanceProperties = 1 << 1,
    TextInstanceProperties = 1 << 2,
    TextSizeChange = 1 << 3,
    Pipeline = 1 << 4,
    Visibility = 1 << 5,
    New = Transform | InstanceProperties | TextInstanceProperties |
          TextSizeChange | Pipeline | Visibility
};

// Bitwise operators for EDirtyType
inline EDirtyType operator|(EDirtyType lhs, EDirtyType rhs) {
    return static_cast<EDirtyType>(static_cast<uint8_t>(lhs) |
                                   static_cast<uint8_t>(rhs));
}

inline EDirtyType operator&(EDirtyType lhs, EDirtyType rhs) {
    return static_cast<EDirtyType>(static_cast<uint8_t>(lhs) &
                                   static_cast<uint8_t>(rhs));
}

inline EDirtyType operator^(EDirtyType lhs, EDirtyType rhs) {
    return static_cast<EDirtyType>(static_cast<uint8_t>(lhs) ^
                                   static_cast<uint8_t>(rhs));
}

inline EDirtyType& operator|=(EDirtyType& lhs, EDirtyType rhs) {
    lhs = lhs | rhs;
    return lhs;
}

inline EDirtyType& operator&=(EDirtyType& lhs, EDirtyType rhs) {
    lhs = lhs & rhs;
    return lhs;
}

inline bool RequireReordering(EDirtyType dirtyType) {
    return (dirtyType & (EDirtyType::TextSizeChange | EDirtyType::Pipeline |
                         EDirtyType::Visibility)) != EDirtyType::None;
}

} // namespace Core