#pragma once
#include <cstddef>

#include "engine/utils/Hashing.h"

namespace Core {
struct GameObjectId {
    uint32_t index{0};
    uint32_t generation{0};
    bool operator!=(const GameObjectId& other) const {
        return index != other.index || generation != other.generation;
    }

    bool operator==(const GameObjectId& other) const {
        return !(*this != other);
    }

    bool operator<(const GameObjectId& other) const {
        return index < other.index && generation < other.generation;
    }

    bool operator<=(const GameObjectId& other) const {
        return index <= other.index && generation <= other.generation;
    }

    bool operator>(const GameObjectId& other) const {
        return index > other.index && generation > other.generation;
    }

    bool operator>=(const GameObjectId& other) const {
        return index >= other.index && generation >= other.generation;
    }
};

struct GameObjectIdHash {
    std::size_t operator()(const GameObjectId& key) const {
        return Utils::CreateHash(key.generation, key.index);
    }
};

} // namespace Core
