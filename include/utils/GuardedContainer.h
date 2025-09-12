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

    T& m_Obj;

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
        using BaseIter = std::unordered_set<::Obj<T>, ObjHasher>::iterator;

        iterator(BaseIter i, BaseIter e) : m_Current(i), m_End(e) {
            skipInvalid();
        }

        std::iterator_traits<BaseIter>::reference operator*() const {
            return *m_Current;
        }

        std::iterator_traits<BaseIter>::pointer operator->() const {
            return (*m_Current);
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
        auto it =
            std::find_if(m_Set.begin(), m_Set.end(), [&obj](const ::Obj<T>& o) {
                return &o.m_Obj == &obj;
            });
        if (it != m_Set.end()) {
            m_Set.erase(it);
        }
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
    std::unordered_set<::Obj<T>, ObjHasher> m_Set;
};