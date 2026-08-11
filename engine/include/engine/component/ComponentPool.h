#pragma once
#include <cstdint>
#include <limits>
#include <vector>

#include "engine/core/Entity.h"

namespace Component {
class IPool {
public:
    virtual ~IPool() = default;

    virtual void Remove(Core::SEntity entity) = 0;
};

template <typename T>
class CPool : public IPool {

public:
    CPool() = default;

    template <typename... Args>
    T& Create(Core::SEntity entity, Args... args) {
        if (entity.id >= mEntityToDataId.size())
            mEntityToDataId.resize(entity.id + 1, kInvalidId);

        mEntityToDataId[entity.id] = static_cast<uint32_t>(mData.size());
        mData.emplace_back(std::forward<Args>(args)...);
        mDataToEntity.push_back(entity);
    }

    void Remove(Core::SEntity entity) override {
        if (!Has(entity))
            return;

        uint32_t denseIdx = mEntityToDataId[entity.id];
        uint32_t lastIdx = mData.size() - 1;
        auto lastEntity = mDataToEntity[lastIdx];

        // swap-and-pop
        mData[denseIdx] = std::move(mData[lastIdx]);
        mDataToEntity[denseIdx] = lastEntity;
        mEntityToDataId[lastEntity.id] = denseIdx;

        mData.pop_back();
        mDataToEntity.pop_back();
        mEntityToDataId[entity.id] = kInvalidId;
    }

    bool Has(Core::SEntity entity) const {
        return entity.id < mEntityToDataId.size() &&
               mEntityToDataId[entity.id] != kInvalidId;
    }

private:
    inline static constexpr uint32_t kInvalidId =
        std::numeric_limits<uint32_t>().max();
    std::vector<T> mData;
    std::vector<Core::SEntity> mDataToEntity;
    std::vector<uint32_t> mEntityToDataId;
};
} // namespace Component
