#pragma once
#include <cstdint>

namespace Core {

enum class EDirtyFlag : uint8_t {
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

// Bitwise operators for EDirtyFlag
inline EDirtyFlag operator|(EDirtyFlag lhs, EDirtyFlag rhs) {
    return static_cast<EDirtyFlag>(static_cast<uint8_t>(lhs) |
                                   static_cast<uint8_t>(rhs));
}

inline EDirtyFlag operator&(EDirtyFlag lhs, EDirtyFlag rhs) {
    return static_cast<EDirtyFlag>(static_cast<uint8_t>(lhs) &
                                   static_cast<uint8_t>(rhs));
}

inline EDirtyFlag operator^(EDirtyFlag lhs, EDirtyFlag rhs) {
    return static_cast<EDirtyFlag>(static_cast<uint8_t>(lhs) ^
                                   static_cast<uint8_t>(rhs));
}

inline EDirtyFlag& operator|=(EDirtyFlag& lhs, EDirtyFlag rhs) {
    lhs = lhs | rhs;
    return lhs;
}

inline EDirtyFlag& operator&=(EDirtyFlag& lhs, EDirtyFlag rhs) {
    lhs = lhs & rhs;
    return lhs;
}

inline bool RequireReordering(EDirtyFlag dirtyType) {
    return (dirtyType & (EDirtyFlag::TextSizeChange | EDirtyFlag::Pipeline |
                         EDirtyFlag::Visibility)) != EDirtyFlag::None;
}

} // namespace Core
