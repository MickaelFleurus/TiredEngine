#pragma once

#include <cstddef>
#include <limits>
#include <tuple>

#include "engine/component/ComponentPool.h"
#include "engine/core/Entity.h"

namespace Component {

template <typename... Components>
class CView {
public:
    explicit CView(CPool<Components>&... p)
        : mPools(p...), mDriver(FindSmallestPool()) {
    }

    explicit CView(std::tuple<CPool<Components>&...> p)
        : mPools(std::move(p)), mDriver(FindSmallestPool()) {
    }

    class Iterator {
    public:
        Iterator(const std::tuple<CPool<Components>&...>& p, IPool* driver,
                 std::size_t startIndex = 0)
            : mPools(p), mDriver(driver), mDriverDataIndex(startIndex) {
            mDriverDataIndex = FindNextIndex();
        }

        Iterator(const Iterator& other) noexcept = default;
        Iterator& operator=(const Iterator& other) noexcept = default;

        template <typename Self>
        decltype(auto) operator*(this Self&& self) {
            return self.GetAll(self.mDriver->GetEntity(self.mDriverDataIndex));
        }

        Iterator& operator++() noexcept {
            mDriverDataIndex++;
            mDriverDataIndex = FindNextIndex();
            return *this;
        }

        bool operator==(const Iterator& other) const noexcept {
            return other.mDriver == this->mDriver &&
                   other.mDriverDataIndex == this->mDriverDataIndex;
        }

        bool operator!=(const Iterator& other) const noexcept {
            return !(*this == other);
        }

    private:
        std::size_t FindNextIndex() {
            for (auto index = mDriverDataIndex; index < mDriver->Size();
                 index++) {
                auto entityDriver = mDriver->GetEntity(index);
                if (MatchesAll(entityDriver)) {
                    return index;
                }
            }
            return mDriver->Size();
        }

        bool MatchesAll(Core::SEntity e) const {
            return std::apply([&](auto&... p) { return (p.Has(e) && ...); },
                              mPools);
        }

        auto GetAll(Core::SEntity e) const {
            return std::apply(
                [&](auto&... p) {
                    return std::tuple<Core::SEntity, const Components&...>(
                        e, p.Get(e)...);
                },
                mPools);
        }

        const std::tuple<CPool<Components>&...>& mPools;
        IPool* mDriver{nullptr};
        std::size_t mDriverDataIndex{};
    };

    Iterator begin() const {
        return Iterator(mPools, mDriver);
    }

    Iterator end() const {
        return Iterator(mPools, mDriver, mDriver->Size());
    }

    IPool* FindSmallestPool() {
        IPool* best = nullptr;
        std::apply(
            [&](auto&... pools) {
                ((best =
                      (best == nullptr || pools.Size() < best->Size() ? &pools
                                                                      : best)),
                 ...);
            },
            mPools);
        return best;
    }

private:
    std::tuple<CPool<Components>&...> mPools;
    IPool* mDriver{nullptr};
};
} // namespace Component
