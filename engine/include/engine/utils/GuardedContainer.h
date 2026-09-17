#pragma once

#include <algorithm>
#include <vector>

#include "engine/utils/Token.h"

namespace {

template <typename T>
class CStoredObject {
public:
    CStoredObject(T& ptr, std::unique_ptr<CToken::CTokenHandle> handler)
        : mStoredItem(ptr), mHandle(std::move(handler)) {
    }
    bool operator==(const CStoredObject& other) const {
        return &mStoredItem == &other.mStoredItem;
    }

    bool IsValid() const {
        return mHandle->IsValid();
    }

    T& mStoredItem;

private:
    std::unique_ptr<CToken::CTokenHandle> mHandle;
};

// struct SStoredObjectHasher {
//     template <typename T>
//     std::size_t operator()(const ::CStoredObject<T>& obj) const {
//         return Utils::CreateHash(&obj.mStoredItem);
//     }
// };

} // namespace

template <typename T>
class CGuardedContainer {
public:
    // Custom iterator that checks validity
    struct iterator {
        using BaseIter = typename std::vector<::CStoredObject<T>>::iterator;

        iterator(BaseIter i, BaseIter e) : mCurrent(i), mEnd(e) {
            skipInvalid();
        }

        T& operator*() {
            return mCurrent->mStoredItem;
        }

        T* operator->() {
            return &mCurrent->mStoredItem;
        }

        const T& operator*() const {
            return mCurrent->mStoredItem;
        }

        const T* operator->() const {
            return &mCurrent->mStoredItem;
        }

        iterator& operator++() {
            ++mCurrent;
            skipInvalid();
            return *this;
        }

        bool operator!=(const iterator& other) const {
            return mCurrent != other.mCurrent;
        }

    private:
        void skipInvalid() {
            while (mCurrent != mEnd && !mCurrent->IsValid()) {
                ++mCurrent;
            }
        }

        BaseIter mCurrent;
        BaseIter mEnd;
    };

    CGuardedContainer() = default;
    ~CGuardedContainer() = default;

    void Add(T& obj, CToken& token) {
        mData.emplace_back(obj, token.GetTokenHandle());
    }

    void Remove(const T& obj) {
        auto it = std::find_if(
            mData.begin(), mData.end(),
            [&obj](const ::CStoredObject<T>& o) { return *o == &obj; });
        if (it != mData.end()) {
            mData.erase(it);
        }
    }

    void Clear() {
        mData.clear();
    }

    iterator begin() {
        return iterator(mData.begin(), mData.end());
    }

    iterator end() {
        return iterator(mData.end(), mData.end());
    }

    iterator cbegin() const {
        return iterator(mData.begin(), mData.end());
    }

    iterator cend() const {
        return iterator(mData.end(), mData.end());
    }

private:
    std::vector<::CStoredObject<T>> mData;
};
