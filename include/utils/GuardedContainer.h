#pragma once

#include "utils/Token.h"
#include <unordered_set>

namespace {

template <typename T>
class CStoredObject {
public:
    CStoredObject(T& ptr, std::unique_ptr<CToken::CTokenHandle> handler)
        : m_StoredItem(ptr), m_Handle(std::move(handler)) {
    }
    bool operator==(const CStoredObject& other) const {
        return &m_StoredItem == &other.m_StoredItem;
    }

    bool IsValid() const {
        return m_Handle->IsValid();
    }

    T& m_StoredItem;

private:
    std::unique_ptr<CToken::CTokenHandle> m_Handle;
};

struct SStoredObjectHasher {
    template <typename T>
    std::size_t operator()(const ::CStoredObject<T>& obj) const {
        // Hash the address of the stored item
        return std::hash<const void*>()(
            static_cast<const void*>(&obj.m_StoredItem));
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

        iterator(BaseIter i, BaseIter e) : m_Current(i), m_End(e) {
            skipInvalid();
        }

        T& operator*() {
            return m_Current->m_StoredItem;
        }

        T* operator->() {
            return &m_Current->m_StoredItem;
        }

        const T& operator*() const {
            return m_Current->m_StoredItem;
        }

        const T* operator->() const {
            return &m_Current->m_StoredItem;
        }

        iterator& operator++() {
            ++m_Current;
            skipInvalid();
            return *this;
        }

        bool operator!=(const iterator& other) const {
            return m_Current != other.m_Current;
        }

    private:
        void skipInvalid() {
            while (m_Current != m_End && !m_Current->IsValid()) {
                ++m_Current;
            }
        }

        BaseIter m_Current;
        BaseIter m_End;
    };

    CGuardedContainer() = default;
    ~CGuardedContainer() = default;

    void Add(T& obj, CToken& token) {
        m_Set.emplace(obj, token.GetTokenHandle());
    }

    void Remove(const T& obj) {
        auto it = std::find_if(
            m_Set.begin(), m_Set.end(),
            [&obj](const ::CStoredObject<T>& o) { return *o == &obj; });
        if (it != m_Set.end()) {
            m_Set.erase(it);
        }
    }

    void Clear() {
        m_Set.clear();
    }

    iterator begin() {
        return std::move(iterator(m_Set.begin(), m_Set.end()));
    }

    iterator end() {
        return std::move(iterator(m_Set.end(), m_Set.end()));
    }

    iterator cbegin() const {
        return iterator(m_Set.begin(), m_Set.end());
    }

    iterator cend() const {
        return iterator(m_Set.end(), m_Set.end());
    }

private:
    std::unordered_set<::CStoredObject<T>, SStoredObjectHasher> m_Set;
};