#pragma once

#include "utils/Token.h"
#include <unordered_set>

namespace {
template <typename T>
class Obj {
public:
    Obj(T& ptr, std::unique_ptr<CToken::CTokenHandle> handler)
        : m_Obj(ptr), m_Handle(std::move(handler)) {
    }

    T& operator*() {
        return m_Obj;
    }
    T* operator->() {
        return &m_Obj;
    }

    bool operator==(const Obj& other) const {
        return &m_Obj == &other.m_Obj;
    }

    const T& operator*() const {
        return m_Obj;
    }
    const T* operator->() const {
        return &m_Obj;
    }

    bool IsValid() const {
        return m_Handle->IsValid();
    }

    T const& m_Obj;

private:
    std::unique_ptr<CToken::CTokenHandle> m_Handle;
};

struct ObjHasher {
    template <typename T>
    std::size_t operator()(const typename ::Obj<T>& obj) const {
        return std::hash<T*>()(const_cast<T*>(&obj.m_Obj));
    }
};

} // namespace

template <typename T>
class CGuardedContainer {
public:
    // Custom iterator that checks validity
    struct iterator {
        using BaseIter = std::unordered_set<T>::iterator;
        BaseIter it, end;

        iterator(BaseIter i, BaseIter e) : it(i), end(e) {
            skipInvalid();
        }

        T& operator*() const {
            return **it;
        }

        T* operator->() const {
            return *it;
        }

        iterator& operator++() {
            ++it;
            skipInvalid();
            return *this;
        }

        bool operator!=(const iterator& other) const {
            return it != other.it;
        }

    private:
        void skipInvalid() {
            while (it != end && !(*it)->IsValid()) {
                ++it;
            }
        }
    };

    CGuardedContainer() = default;
    ~CGuardedContainer() = default;

    void Add(T& obj, CToken& token) {
        m_Set.emplace(obj, token.GetTokenHandle());
    }

    void Remove(const T& obj) {
        auto it =
            std::find_if(m_Set.begin(), m_Set.end(), [&obj](const ::Obj<T>& o) {
                return &o.m_Obj == &obj;
            });
        if (it != m_Set.end()) {
            m_Set.erase(it);
        }
    }

    auto begin() {
        return m_Set.begin();
    }

    auto end() {
        return m_Set.end();
    }

    auto cbegin() const {
        return m_Set.begin();
    }

    auto cend() const {
        return m_Set.end();
    }

private:
    std::unordered_set<::Obj<T>, ObjHasher> m_Set;
};