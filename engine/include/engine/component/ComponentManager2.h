#pragma once

#include <memory>
#include <tuple>
#include <vector>

#include "engine/component/ComponentPool.h"
#include "engine/core/Entity.h"

namespace Component {

inline uint32_t nextComponentTypeId() {
    static uint32_t counter = 0;
    return counter++;
}

template <typename T>
uint32_t componentTypeId() {
    static const uint32_t id =
        nextComponentTypeId(); // computed once per T, cached
    return id;
}

class CManager {
public:
    explicit CManager() = default;

    template <typename T>
    CPool<T>& GetPool() {
        uint32_t id = componentTypeId<T>();
        if (id >= mPools.size())
            mPools.resize(id + 1);
        if (!mPools[id])
            mPools[id] = std::make_unique<CPool<T>>();
        return static_cast<CPool<T>&>(*mPools[id]);
    }

    template <typename... C>
    std::tuple<CPool<C>&...> GetPools() {
        return std::forward_as_tuple((GetPool<C>())...);
    }

    void DestroyComponents(Core::SEntity entity);

private:
    std::vector<std::unique_ptr<IPool>> mPools;
};
} // namespace Component
