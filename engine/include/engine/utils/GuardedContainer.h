#pragma once

#include "engine/utils/Hashing.h"
#include "engine/utils/Token.h"
#include <algorithm>
#include <unordered_set>

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

struct SStoredObjectHasher {
    template <typename T>
    std::size_t operator()(const ::CStoredObject<T>& obj) const {
        return Utils::CreateHash(&obj.mStoredItem);
    }
};

} // namespace

template <typename T>
class CGuardedContainer {
public:
    // Custom iterator that checks validity
    struct iterator {
        using BaseIter = std::unordered_set<::CStoredObject<T>,
                                            SStoredObjectHasher>::iterator;

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
        mSet.emplace(obj, token.GetTokenHandle());
    }

    void Remove(const T& obj) {
        auto it = std::find_if(
            mSet.begin(), mSet.end(),
            [&obj](const ::CStoredObject<T>& o) { return *o == &obj; });
        if (it != mSet.end()) {
            mSet.erase(it);
        }
    }

    void Clear() {
        mSet.clear();
    }

    iterator begin() {
        return std::move(iterator(mSet.begin(), mSet.end()));
    }

    iterator end() {
        return std::move(iterator(mSet.end(), mSet.end()));
    }

    iterator cbegin() const {
        return iterator(mSet.begin(), mSet.end());
    }

    iterator cend() const {
        return iterator(mSet.end(), mSet.end());
    }

private:
    std::unordered_set<::CStoredObject<T>, SStoredObjectHasher> mSet;
};